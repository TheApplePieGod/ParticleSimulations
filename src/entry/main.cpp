#include <Diamond/diamond.h>
#include <ParticleSimulations/util/basic.h>
#include <ParticleSimulations/slime.h>
#include <ParticleSimulations/fluid.h>

diamond* Engine = new diamond();

int main(int argc, char** argv)
{
    Engine->Initialize(1000, 1000, 1000, 1000, "Particle Simulations", "../shaders/default.vert.spv", "../shaders/default.frag.spv");

    simulation_slime* slimeSimulation = new simulation_slime();
    slimeSimulation->Initialize(500000, 2560, 1440);

    simulation_fluid* fluidSimulation = new simulation_fluid();
    fluidSimulation->Initialize(1000, 1000);

    while (Engine->IsRunning())
    {
        Engine->BeginFrame(diamond_camera_mode::OrthographicViewportIndependent, glm::vec2(500.f, 500.f), Engine->GenerateViewMatrix(glm::vec2(0.f, 0.f)), -1);

        //slimeSimulation->Run();
        //slimeSimulation->RenderGUI();

        fluidSimulation->Run();
        fluidSimulation->RenderGUI();

        Engine->EndFrame({ 0.f, 0.f, 0.f, 1.f });
    }

    Engine->Cleanup();

    return 0;
}