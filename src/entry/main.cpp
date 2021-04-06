#include <Diamond/diamond.h>
#include <iostream>
#include <ParticleSimulations/util/basic.h>
#include <gtc/matrix_transform.hpp>
#include <chrono>
#include <ParticleSimulations/slime.h>

int main(int argc, char** argv)
{
    diamond* Engine = new diamond();

    int imageSizeX = 1920;
    int imageSizeY = 1080;
    int agentCount = 250000;    
    Engine->Initialize(imageSizeX, imageSizeY, 1000, 1000, "Particle Simulations", "../shaders/slime.vert.spv", "../shaders/slime.frag.spv");

    std::array<diamond_compute_buffer_info, 1> cpBuffers {
        diamond_compute_buffer_info("agentData", sizeof(slime_agent) * agentCount, false, true, false),
    };
    std::array<diamond_compute_image_info, 1> cpImages {
        diamond_compute_image_info("trailMap", imageSizeX, imageSizeY, 32)
    };
    diamond_compute_pipeline_create_info cpCreateInfo = {};
    cpCreateInfo.enabled = true;
    cpCreateInfo.imageCount = cpImages.size();
    cpCreateInfo.imageInfoList = cpImages.data();
    cpCreateInfo.bufferCount = cpBuffers.size();
    cpCreateInfo.bufferInfoList = cpBuffers.data();
    cpCreateInfo.computeShaderPath = "../shaders/slime.comp.spv";
    cpCreateInfo.groupCountX = ceil(agentCount / 64);
    cpCreateInfo.shouldBlockCPU = true;
    cpCreateInfo.usePushConstants = true;
    cpCreateInfo.pushConstantsDataSize = sizeof(slime_constants);
    cpCreateInfo.preRunSyncFlags = {
        0, 0, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
    };
    cpCreateInfo.postRunSyncFlags = {
        VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
    };
    Engine->CreateComputePipeline(cpCreateInfo);

    cpCreateInfo.bufferCount = 0;
    cpCreateInfo.shouldBlockCPU = false;
    cpCreateInfo.computeShaderPath = "../shaders/trailmap.comp.spv";
    cpCreateInfo.groupCountX = ceil(imageSizeX / 8);
    cpCreateInfo.groupCountY = ceil(imageSizeY / 8);
    Engine->CreateComputePipeline(cpCreateInfo);

    // initialize agent data
    std::vector<slime_agent> agentData(agentCount);
    for (int i = 0; i < agentData.size(); i++)
    {
        f32 angle = (rand() / static_cast<f32>(RAND_MAX)) * Pi32 * 2;
        glm::vec2 angleVec = glm::vec2(cos(angle), sin(angle));
        agentData[i].position = glm::vec2(imageSizeX / 2.f, imageSizeY / 2.f) + angleVec * (0.5f * (rand() % imageSizeY));
        agentData[i].angle = angle + Pi32;
    }
    Engine->MapComputeData(0, 0, 0, sizeof(slime_agent) * agentCount, agentData.data()); // map inital data

    int frameCount = 0;
    slime_constants constants = {};
    int scale = 30;
    constants.numAgents = agentCount;
    constants.moveSpeed = 20 * scale;
    constants.evaporateSpeed = 0.2f * scale;
    constants.diffuseSpeed = 3.f * scale;
    constants.turnSpeed = Pi32 * 2.0 * scale;
    while (Engine->IsRunning())
    {
        Engine->BeginFrame(diamond_camera_mode::OrthographicViewportIndependent, glm::vec2(500.f, 500.f), Engine->GenerateViewMatrix(glm::vec2(0.f, 0.f)), -1);

        constants.deltaTime = Engine->FrameDelta() / 1000.0;//std::max(Engine->FrameDelta() / 1000.0, 0.00005); // convert to seconds
        Engine->RunComputeShader(0, frameCount == 0, &constants);
        Engine->RunComputeShader(1, false, &constants);

        diamond_transform quadTransform;
        quadTransform.location = { 0.f, 0.f };
        quadTransform.rotation = 0.f;
        quadTransform.scale = { 3000.f, 3000.f };
        Engine->DrawQuad(1, quadTransform);

        Engine->EndFrame({ 0.f, 0.f, 0.f, 1.f });
        frameCount++;
        //std::cout << "FPS: " << Engine->FPS() << std::endl;
    }

    Engine->Cleanup();

    return 0;
}