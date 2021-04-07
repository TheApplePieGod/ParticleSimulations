#pragma once
#include <vec2.hpp>
#include <vec3.hpp>
#include <ParticleSimulations/util/basic.h>
#include <vector>

struct fluid_cell
{
    int type;
    f32 mass;
};

struct fluid_constants
{
    int numCells;
    int mapWidth;
    int mapHeight;
    int iterations;
    f32 diffusion;
    f32 viscosity;
    f32 deltaTime;
    bool dirty;
};

class simulation_fluid
{
public:
    void Initialize(int imageSizeX, int imageSizeY);
    void Run();
    void RenderGUI();

private:
    std::vector<fluid_cell> cellData;
    fluid_constants settings;

    int imageSizeX;
    int imageSizeY;
    int simulateComputeIndex;
    int copyDataComputeIndex;
    int textureIndex;
};