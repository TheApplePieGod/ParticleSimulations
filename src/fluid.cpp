#include <ParticleSimulations/fluid.h>
#include <ParticleSimulations/input.h>
#include <Diamond/diamond.h>
#include <imgui.h>

extern diamond* Engine;
extern input_manager* InputManager;

void simulation_fluid::Initialize(int _numParticles)
{
    Engine->UpdateVertexStructInfo(sizeof(fluid_particle), VK_PRIMITIVE_TOPOLOGY_POINT_LIST, fluid_particle::GetAttributeDescriptions, fluid_particle::GetBindingDescription);

    numParticles = _numParticles;

    particleData.resize(numParticles);
    forceInputs.resize(numParticles);

    std::array<diamond_compute_buffer_info, 2> cpBuffers {
        diamond_compute_buffer_info("fluidParticleData", sizeof(fluid_particle) * numParticles, true, true),
        diamond_compute_buffer_info("fluidForceInputs", sizeof(glm::vec4) * numParticles, false, true)
    };
    diamond_compute_pipeline_create_info cpCreateInfo = {};
    cpCreateInfo.enabled = true;
    cpCreateInfo.bufferCount = cpBuffers.size();
    cpCreateInfo.bufferInfoList = cpBuffers.data();
    cpCreateInfo.computeShaderPath = "../shaders/fluid_compute_density_pressure.comp.spv";
    cpCreateInfo.groupCountX = ceil(numParticles / 64) + 1;
    cpCreateInfo.usePushConstants = true;
    cpCreateInfo.pushConstantsDataSize = sizeof(fluid_constants);
    densityPressureComputeIndex = Engine->CreateComputePipeline(cpCreateInfo);

    cpCreateInfo.computeShaderPath = "../shaders/fluid_compute_force.comp.spv";
    forceComputeIndex = Engine->CreateComputePipeline(cpCreateInfo);

    cpCreateInfo.computeShaderPath = "../shaders/fluid_integrate.comp.spv";
    integrateComputeIndex = Engine->CreateComputePipeline(cpCreateInfo);

    FillScreen();

    settings.numParticles = numParticles;
    settings.viscosity = 3000.f;
    settings.mass = 0.02f;
    settings.radius = 0.005f;
    settings.wallDamping = 0.3f;
    settings.density = 10000.f;
    settings.stiffness = 5000.f;
    settings.dragSpeed = 10.f;
    settings.colliderIndex = 0;
    settings.dirty = true;
}

void simulation_fluid::Run()
{
    Engine->BeginFrame(diamond_camera_mode::OrthographicViewportIndependent, glm::vec2(500.f, 500.f), Engine->GenerateViewMatrix(glm::vec2(0.f, 0.f)), densityPressureComputeIndex, 0);
    settings.deltaTime = Engine->FrameDelta() / 1000.0; // convert to seconds

    if (settings.dirty)
    {
        Engine->MapComputeData(densityPressureComputeIndex, 0, 0, sizeof(fluid_particle) * numParticles, particleData.data()); // map inital data
        Engine->UploadComputeData(densityPressureComputeIndex, 0);
    }
    
    Engine->RunComputeShader(densityPressureComputeIndex, &settings);
    Engine->RunComputeShader(forceComputeIndex, &settings);

    if (InputManager->IsMouseDown(1))
    { 
        Engine->RetrieveComputeData(integrateComputeIndex, 0, 0, sizeof(fluid_particle) * numParticles, particleData.data());

        glm::vec2 mousePos = InputManager->GetMouseScreenPosition();
        if (lastMousePos != mousePos)
        {
            glm::vec2 trajectory = glm::normalize(mousePos - lastMousePos);
            lastMousePos = mousePos;

            glm::vec2 localMousePos = (mousePos / Engine->GetWindowSize()) * 2.f - 1.f;
            f32 scalar = 100000000.f * settings.dragSpeed;
            for (int i = 0; i < particleData.size(); i++)
            {
                if (length(particleData[i].position - localMousePos) <= 0.2)
                {
                    glm::vec2 force = trajectory * scalar;
                    forceInputs[i] = glm::vec4(force, 0.f, 0.f);
                }
                else
                    forceInputs[i] = glm::vec4(0.f);
            }
        }

        Engine->MapComputeData(integrateComputeIndex, 1, 0, sizeof(glm::vec4) * numParticles, forceInputs.data());
        Engine->UploadComputeData(integrateComputeIndex, 1);
    }

    Engine->RunComputeShader(integrateComputeIndex, &settings);

    Engine->DownloadComputeData(integrateComputeIndex, 0);

    settings.dirty = false;

    Engine->DrawFromCompute(numParticles);
}

void simulation_fluid::RenderGUI()
{
    if (ImGui::Begin("Fluid Settings"));
    {
        ImGui::Text("Particle Count: %d", settings.numParticles);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Viscosity:");
        ImGui::SameLine();
        f32 Min = 0.f;
        f32 Max = 50000.f;
        ImGui::DragScalar("##Viscosity", ImGuiDataType_Float, &settings.viscosity, 100.f, &Min, &Max, "%f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Density:");
        ImGui::SameLine();
        ImGui::DragScalar("##Density", ImGuiDataType_Float, &settings.density, 100.f, &Min, &Max, "%f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Stiffness:");
        ImGui::SameLine();
        ImGui::DragScalar("##Stiffness", ImGuiDataType_Float, &settings.stiffness, 100.f, &Min, &Max, "%f");

        Max = 50.f;
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Drag Speed:");
        ImGui::SameLine();
        ImGui::DragScalar("##DragSpeed", ImGuiDataType_Float, &settings.dragSpeed, 1.f, &Min, &Max, "%f");

        int intMin = 0;
        int intMax = 4;
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Collider Presets:");
        ImGui::SameLine();
        ImGui::DragScalar("##ColliderPresets", ImGuiDataType_S32, &settings.colliderIndex, 1, &intMin, &intMax, "%d");

        if (ImGui::Button("Reset"))
        {
            settings.dirty = true;
            FillScreen();
        }

        ImGui::End();
    }
}

void simulation_fluid::FillScreen()
{
    glm::vec2 startingPos = glm::vec2(-1, -1);
    f32 particleRadius = 0.005f;
    for (int i = 0; i < numParticles; i++)
    {
        particleData[i].position = startingPos;
        particleData[i].velocity = glm::vec2(0.f, 0.f);
        particleData[i].force = glm::vec2(0.f, 0.f);
        particleData[i].density = 0.f;
        particleData[i].pressure = 0.f;
        startingPos.x += particleRadius * 2.f;
        if (startingPos.x >= 1)
        {
            startingPos.x = -1.f;
            startingPos.y += particleRadius * 2.f;
        }
    }
}