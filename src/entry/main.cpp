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
    Engine->Initialize(2560, 1440, "Particle Simulations", "../images/default-texture.png");

    simulation sim = simulation::Slime;

    simulation_slime* slimeSimulation = new simulation_slime();
    slimeSimulation->Initialize(100000, 2560, 1440);
    slimeSimulation->Focus();

    simulation_fluid* fluidSimulation = new simulation_fluid();
    fluidSimulation->Initialize(20000);

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

        // general gui
        if (ImGui::Begin("General Settings"))
        {
            if (ImGui::Button("Run Slime Simulation"))
            {
                slimeSimulation->Focus();
                sim = simulation::Slime;
            }
            if (ImGui::Button("Run Fluid Simulation"))
            {
                fluidSimulation->Focus();
                sim = simulation::Fluid;
            }

            ImGui::End();
        }

        Engine->EndFrame({ 0.f, 0.f, 0.f, 1.f });
        InputManager->ClearJustPressedFlags();
    }

    Engine->Cleanup();

    return 0;
}