#include <ParticleSimulations/slime.h>
#include <Diamond/diamond.h>
#include <imgui/imgui.h>

extern diamond* Engine;

void simulation_slime::Initialize(int _agentCount, int _imageSizeX, int _imageSizeY)
{
    agentCount = _agentCount;
    imageSizeX = _imageSizeX;
    imageSizeY = _imageSizeY;

    agentData.resize(agentCount);

    std::array<diamond_compute_buffer_info, 1> cpBuffers {
        diamond_compute_buffer_info("slimeAgentData", sizeof(slime_agent) * agentCount, false, true),
    };
    std::array<diamond_compute_image_info, 1> cpImages {
        diamond_compute_image_info("slimeTrailMap", imageSizeX, imageSizeY, 32)
    };
    diamond_compute_pipeline_create_info cpCreateInfo = {};
    cpCreateInfo.imageCount = cpImages.size();
    cpCreateInfo.imageInfoList = cpImages.data();
    cpCreateInfo.bufferCount = cpBuffers.size();
    cpCreateInfo.bufferInfoList = cpBuffers.data();
    cpCreateInfo.computeShaderPath = "../shaders/slime.comp.spv";
    cpCreateInfo.groupCountX = ceil(agentCount / 64) + 1;
    cpCreateInfo.usePushConstants = true;
    cpCreateInfo.pushConstantsDataSize = sizeof(slime_constants);
    slimeComputeIndex = Engine->CreateComputePipeline(cpCreateInfo);

    cpCreateInfo.bufferCount = 0;
    cpCreateInfo.computeShaderPath = "../shaders/trailmap.comp.spv";
    cpCreateInfo.groupCountX = ceil(imageSizeX / 8);
    cpCreateInfo.groupCountY = ceil(imageSizeY / 8);
    trailmapComputeIndex = Engine->CreateComputePipeline(cpCreateInfo);

    diamond_graphics_pipeline_create_info gpCreateInfo = {};
    gpCreateInfo.vertexShaderPath = "../shaders/default.vert.spv";
    gpCreateInfo.fragmentShaderPath = "../shaders/default.frag.spv";
    gpCreateInfo.maxVertexCount = 1000;
    gpCreateInfo.maxIndexCount = 1000;
    graphicsPipelineIndex = Engine->CreateGraphicsPipeline(gpCreateInfo);

    textureIndex = Engine->GetComputeTextureIndex(slimeComputeIndex, 0);

    StartAgentsFromOutside();

    settings.numAgents = agentCount;
    settings.moveSpeed = 50;
    settings.evaporateSpeed = 0.2f;
    settings.diffuseSpeed = 3.f;
    settings.turnSpeed = 6.3f;
    settings.sensorOffset = 35.0f;
    settings.sensorAngle = 0.5f;
    settings.sensorSize = 1;
    settings.dirty = true;
    settings.color = glm::vec4(1.f, 0.f, 0.f, 1.f);
}

void simulation_slime::Recreate(int _agentCount, int _imageSizeX, int _imageSizeY)
{
    Engine->DeleteComputePipeline(slimeComputeIndex);
    Engine->DeleteComputePipeline(trailmapComputeIndex);
    Engine->DeleteGraphicsPipeline(graphicsPipelineIndex);

    Initialize(_agentCount, _imageSizeX, _imageSizeY);
}

void simulation_slime::Run()
{
    if (recreate)
    {
        recreate = false;
        Recreate(agentCount, imageSizeX, imageSizeY);
    }

    Engine->BeginFrame();
    Engine->UpdateCameraViewMode(diamond_camera_mode::OrthographicViewportIndependent, glm::vec2(500.f, 500.f)); // do every frame to account for screen resizes
    Engine->SetCameraViewMatrix(Engine->GenerateViewMatrix(glm::vec3(0.f, 0.f, 5.f)));
    settings.deltaTime = Engine->FrameDeltaRaw() / 1000.0; // convert to seconds

    Engine->SetGraphicsPipeline(graphicsPipelineIndex);

    if (settings.dirty)
    {
        Engine->UploadComputeData(slimeComputeIndex, 0);
    }

    Engine->RunComputeShader(slimeComputeIndex, &settings);
    Engine->RunComputeShader(trailmapComputeIndex, &settings);
    settings.dirty = false;

    diamond_transform quadTransform;
    quadTransform.location = { 0.f, 0.f };
    quadTransform.rotation = 0.f;
    quadTransform.scale = { 3000.f, 3000.f };
    Engine->DrawQuad(textureIndex, quadTransform);
}

void simulation_slime::Focus()
{
    Engine->SetWindowSize(glm::vec2(imageSizeX, imageSizeY));
}

void simulation_slime::RenderGUI()
{
    if (ImGui::Begin("Slime Settings"));
    {
        ImGui::Text("Real Agent Count: %d", settings.numAgents);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Evaporate Speed:");
        ImGui::SameLine();
        f32 Min = 0.f;
        f32 Max = 1000.f;
        ImGui::DragScalar("##EvaporateSpeed", ImGuiDataType_Float, &settings.evaporateSpeed, 0.1f, &Min, &Max, "%f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Diffuse Speed:");
        ImGui::SameLine();
        ImGui::DragScalar("##DiffuseSpeed", ImGuiDataType_Float, &settings.diffuseSpeed, 0.1f, &Min, &Max, "%f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Turn Speed:");
        ImGui::SameLine();
        ImGui::DragScalar("##TurnSpeed", ImGuiDataType_Float, &settings.turnSpeed, 0.1f, &Min, &Max, "%f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Move Speed:");
        ImGui::SameLine();
        ImGui::DragScalar("##MoveSpeed", ImGuiDataType_Float, &settings.moveSpeed, 10.f, &Min, &Max, "%f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Sensor Distance:");
        ImGui::SameLine();
        ImGui::DragScalar("##SensorDistance", ImGuiDataType_Float, &settings.sensorOffset, 0.1f, &Min, &Max, "%f");

        Max = Pi32;
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Sensor Angle:");
        ImGui::SameLine();
        ImGui::DragScalar("##SensorAngle", ImGuiDataType_Float, &settings.sensorAngle, 0.1f, &Min, &Max, "%f");

        int intMin = 0;
        int intMax = 10;
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Sensor Size:");
        ImGui::SameLine();
        ImGui::DragScalar("##SensorSize", ImGuiDataType_S32, &settings.sensorSize, 1, &intMin, &intMax, "%d");

        ImGui::ColorPicker4("##ColorPicker", (float*)&settings.color);

        if (ImGui::Button("Reset From Center"))
        {
            settings.dirty = true;
            StartAgentsFromCenter();
        }

        if (ImGui::Button("Reset From Outside"))
        {
            settings.dirty = true;
            StartAgentsFromOutside();
        }

        ImGui::Separator();
        intMin = 0;
        intMax = 1000000;
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Agent Count:");
        ImGui::SameLine();
        ImGui::DragScalar("##AgentCount", ImGuiDataType_S32, &agentCount, 1000, &intMin, &intMax, "%d");

        ImGui::NewLine();

        ImGui::Text("Click to update settings below divider");
        if (ImGui::Button("Restart Sim"))
        {
            recreate = true;
        }

        ImGui::End();
    }
}

void simulation_slime::StartAgentsFromCenter()
{
    for (int i = 0; i < agentData.size(); i++)
    {
        // in going out
        agentData[i].position = glm::vec2(imageSizeX / 2.f, imageSizeY / 2.f);
        agentData[i].angle = (rand() / static_cast<f32>(RAND_MAX)) * Pi32 * 2;
    }
    Engine->MapComputeData(slimeComputeIndex, 0, 0, sizeof(slime_agent) * agentCount, agentData.data()); // map inital data
}

void simulation_slime::StartAgentsFromOutside()
{
    for (int i = 0; i < agentData.size(); i++)
    {
        // out going in
        f32 angle = (rand() / static_cast<f32>(RAND_MAX)) * Pi32 * 2;
        glm::vec2 angleVec = glm::vec2(cos(angle), sin(angle));
        agentData[i].position = glm::vec2(imageSizeX / 2.f, imageSizeY / 2.f) + angleVec * (0.5f * (rand() % imageSizeY));
        agentData[i].angle = angle + Pi32;
    }
    Engine->MapComputeData(slimeComputeIndex, 0, 0, sizeof(slime_agent) * agentCount, agentData.data()); // map inital data
}