#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <ParticleSimulations/util/basic.h>
#include <vector>
#include <Diamond/diamond.h>

struct fluid_particle
{
    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(fluid_particle);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    // Internal use
    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(fluid_particle, position);
        
        return attributeDescriptions;
    }

    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec2 force;
    f32 density;
    f32 pressure;
};

struct fluid_constants
{
    int numParticles;
    f32 viscosity;
    f32 mass;
    f32 radius;
    f32 wallDamping;
    f32 density;
    f32 stiffness;
    f32 deltaTime;
    f32 dragSpeed;
    int colliderIndex;
    bool dirty;
};

class simulation_fluid
{
public:
    void Initialize(int numParticles);
    void Recreate(int numParticles);
    void Run();
    void Focus();
    void RenderGUI();

private:
    std::vector<fluid_particle> particleData;
    std::vector<glm::vec4> forceInputs;
    fluid_constants settings;

    glm::vec2 lastMousePos = glm::vec2(INFINITY, INFINITY);

    int numParticles;
    int densityPressureComputeIndex;
    int forceComputeIndex;
    int integrateComputeIndex;
    int graphicsPipelineIndex;

    bool recreate = false;

    void FillScreen();
};