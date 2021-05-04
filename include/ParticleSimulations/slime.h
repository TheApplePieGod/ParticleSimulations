#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <ParticleSimulations/util/basic.h>
#include <vector>

struct slime_constants
{
    int numAgents;
    f32 moveSpeed;
    f32 diffuseSpeed;
    f32 evaporateSpeed;
    f32 turnSpeed;
    f32 sensorOffset;
    f32 sensorAngle;
    int sensorSize;
    f32 deltaTime;
    int dirty;
    glm::vec2 padding;
    glm::vec4 color;
};

struct slime_agent
{
    glm::vec2 position;
    f32 angle;
    f32 padding;
};

class simulation_slime
{
public:
    void Initialize(int agentCount, int imageSizeX, int imageSizeY);
    void Recreate(int agentCount, int imageSizeX, int imageSizeY);
    void Run();
    void Focus();
    void RenderGUI();

private:
    std::vector<slime_agent> agentData;
    slime_constants settings;

    int agentCount;
    int imageSizeX;
    int imageSizeY;
    int slimeComputeIndex;
    int trailmapComputeIndex;
    int graphicsPipelineIndex;
    int textureIndex;

    bool recreate = false;

    void StartAgentsFromCenter();
    void StartAgentsFromOutside();
};