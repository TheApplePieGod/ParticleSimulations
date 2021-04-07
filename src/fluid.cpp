#include <ParticleSimulations/fluid.h>
#include <Diamond/diamond.h>
#include <imgui.h>

extern diamond* Engine;

void simulation_fluid::Initialize(int _imageSizeX, int _imageSizeY)
{
    imageSizeX = _imageSizeX;
    imageSizeY = _imageSizeY;

    cellData.resize(imageSizeX * imageSizeY);

    std::array<diamond_compute_buffer_info, 2> cpBuffers {
        diamond_compute_buffer_info("fluidCellData", sizeof(fluid_cell) * cellData.size(), false, true, false),
        diamond_compute_buffer_info("fluidTempCellData", sizeof(fluid_cell) * cellData.size(), false, true, false),
    };
    std::array<diamond_compute_image_info, 1> cpImages {
        diamond_compute_image_info("fluidTexture", imageSizeX, imageSizeY, 32)
    };
    diamond_compute_pipeline_create_info cpCreateInfo = {};
    cpCreateInfo.enabled = true;
    cpCreateInfo.imageCount = cpImages.size();
    cpCreateInfo.imageInfoList = cpImages.data();
    cpCreateInfo.bufferCount = cpBuffers.size();
    cpCreateInfo.bufferInfoList = cpBuffers.data();
    cpCreateInfo.computeShaderPath = "../shaders/fluid.comp.spv";
    cpCreateInfo.groupCountX = ceil(imageSizeX / 8);
    cpCreateInfo.groupCountY = ceil(imageSizeY / 8);
    cpCreateInfo.shouldBlockCPU = true;
    cpCreateInfo.usePushConstants = true;
    cpCreateInfo.pushConstantsDataSize = sizeof(fluid_constants);
    cpCreateInfo.preRunSyncFlags = {
        0, 0, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
    };
    cpCreateInfo.postRunSyncFlags = {
        VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
    };
    simulateComputeIndex = Engine->CreateComputePipeline(cpCreateInfo);

    cpCreateInfo.computeShaderPath = "../shaders/fluidCopy.comp.spv";
    copyDataComputeIndex = Engine->CreateComputePipeline(cpCreateInfo);

    textureIndex = Engine->ComputePipelineFirstTextureIndex(simulateComputeIndex);

    for (int i = imageSizeX * 5; i < imageSizeX * imageSizeY; i++)
    {
        cellData[i].type = 0;
        cellData[i].mass = 1.0;
    }
    Engine->MapComputeData(simulateComputeIndex, 0, 0, sizeof(fluid_cell) * cellData.size(), cellData.data()); // map inital data

    settings.numCells = cellData.size();
    settings.mapWidth = imageSizeX;
    settings.mapHeight = imageSizeY;
    settings.iterations = 4;
    settings.diffusion = 1.f;
    settings.viscosity = 1.f;
    settings.dirty = true;
}

void simulation_fluid::Run()
{
    settings.deltaTime = Engine->FrameDelta() / 1000.0; // convert to seconds

    Engine->RunComputeShader(simulateComputeIndex, settings.dirty, &settings);
    Engine->RunComputeShader(copyDataComputeIndex, false, &settings);
    settings.dirty = false;

    diamond_transform quadTransform;
    quadTransform.location = { 0.f, 0.f };
    quadTransform.rotation = 0.f;
    quadTransform.scale = { 3000.f, 3000.f };
    Engine->DrawQuad(textureIndex, quadTransform);
}

void simulation_fluid::RenderGUI()
{
    if (ImGui::Begin("Fluid Settings"));
    {


        ImGui::End();
    }
}