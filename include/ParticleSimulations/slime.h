#pragma once
#include <vec2.hpp>
#include <ParticleSimulations/util/basic.h>

struct slime_constants
{
    int numAgents;
    f32 moveSpeed;
    f32 diffuseSpeed;
    f32 evaporateSpeed;
    f32 turnSpeed;
    f32 deltaTime;
};

struct slime_agent
{
    glm::vec2 position;
    f32 angle;
    f32 padding;
};