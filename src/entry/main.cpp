#define DIAMOND_IMGUI 1
#include <Diamond/diamond.h>
#include <iostream>
#include <ParticleSimulations/util/basic.h>
#include <gtc/matrix_transform.hpp>
#include <chrono>
#include <ParticleSimulations/slime.h>

diamond* Engine = new diamond();

int main(int argc, char** argv)
{
    Engine->Initialize(1920, 1080, 1000, 1000, "Particle Simulations", "../shaders/slime.vert.spv", "../shaders/slime.frag.spv");

    simulation_slime* slimeSimulation = new simulation_slime();
    slimeSimulation->Initialize(500000, 2560, 1440);

    while (Engine->IsRunning())
    {
        Engine->BeginFrame(diamond_camera_mode::OrthographicViewportIndependent, glm::vec2(500.f, 500.f), Engine->GenerateViewMatrix(glm::vec2(0.f, 0.f)), -1);

        slimeSimulation->Run();
        slimeSimulation->RenderGUI();

        Engine->EndFrame({ 0.f, 0.f, 0.f, 1.f });
    }

    Engine->Cleanup();

    return 0;
}