#include <Diamond/diamond.h>
#include <ParticleSimulations/util/basic.h>
#include <ParticleSimulations/slime.h>
#include <ParticleSimulations/fluid.h>
#include <ParticleSimulations/input.h>
#include <iostream>

diamond* Engine = new diamond();
input_manager* InputManager = new input_manager();

enum simulation
{
    Slime,
    Fluid
};

int main(int argc, char** argv)
{
    simulation sim = simulation::Fluid;

    simulation_slime* slimeSimulation = new simulation_slime();
    simulation_fluid* fluidSimulation = new simulation_fluid();

    switch (sim)
    {
        case simulation::Slime:
        {
            Engine->Initialize(2560, 1440, 1000000, 50, "Particle Simulations", "../shaders/default.vert.spv", "../shaders/default.frag.spv");
            slimeSimulation->Initialize(100000, 2560, 1440);
        } break;

        case simulation::Fluid:
        {
            Engine->Initialize(1000, 1000, 1000000, 50, "Particle Simulations", "../shaders/fluid.vert.spv", "../shaders/fluid.frag.spv");
            fluidSimulation->Initialize(20000);
        } break;

        default:
        { return 0; }
    }

    InputManager->Initialize();

    while (Engine->IsRunning())
    {
        InputManager->Tick();
        switch (sim)
        {
            case simulation::Slime:
            {
                slimeSimulation->Run();
                slimeSimulation->RenderGUI();
            } break;

            case simulation::Fluid:
            {
                fluidSimulation->Run();
                fluidSimulation->RenderGUI();
            } break;

            default:
            {} break;
        }

        Engine->EndFrame({ 0.f, 0.f, 0.f, 1.f });
        InputManager->ClearJustPressedFlags();
    }

    Engine->Cleanup();

    return 0;
}