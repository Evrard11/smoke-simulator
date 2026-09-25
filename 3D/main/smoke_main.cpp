//
// Created by Evrard on 11/04/2026.
//

#include "smoke_main.h"

//const int TOTAL_FRAMES = 300;
const int PRESSURE_ITERS = 5;
const float BUOYANCY = 0.8f;
const float DISSIPATION = 0.999f;

void inject_smoke(FluidGrid3D& grid) {
    const int srcX = grid.cellCountX / 2;
    const int srcY = 2;
    const int srcZ = grid.cellCountZ / 2 + 5;

    int radius = 10;
    for (int dx = -radius; dx <= radius; dx++) {
        for (int dz = -radius; dz <= radius; dz++) {
            const int ix = srcX + dx;
            const int iz = srcZ + dz;
            if (ix < 1 || ix >= grid.cellCountX - 1) continue;
            if (iz < 1 || iz >= grid.cellCountZ - 1) continue;

            // Smoke concentration
            grid.densityMap[ix][srcY    ][iz] = 1.0f;
            grid.densityMap[ix][srcY + 1][iz] = 0.8f;

            // Initial upward impulse
            grid.velocitiesY[ix][srcY + 1][iz] = 2.0f;
            grid.velocitiesY[ix][srcY + 2][iz] = 2.0f;
        }
    }
}

void smoke_step(FluidGrid3D& grid, int frame_nb) {
    for (int frame = 0; frame < frame_nb; frame++)
    {
        inject_smoke(grid);
        grid.AdvectVelocity();
        grid.ApplyBuoyancy(BUOYANCY);
        for (int i = 0; i < PRESSURE_ITERS; i++)
            grid.SolvePressure();
        grid.UpdateVelocities();
        grid.AdvectDensity();
        grid.DissipeDensity(DISSIPATION);
    }
}

FluidGrid3D smoke_main() {
    FluidGrid3D grid(80, 80, 80, 1.f);
    // auto grid = std::make_unique<FluidGrid3D>(80, 80, 120, 5.f);

    std::cout << "Grid : " << grid.cellCountX << " x " << grid.cellCountY << " x " << grid.cellCountZ << "\n";

    grid.InitSolidBorders();

    for (int x = 0; x < grid.cellCountX; x++)
        for (int y = 0; y < grid.cellCountY; y++)
            for (int z = 0; z < grid.cellCountZ; z++)
                grid.densityMap[x][y][z] = 0.1f;

    // Static 3D obstacle near the center of the domain
    // const int cx = grid.cellCountX / 2;
    // const int cy = grid.cellCountY / 2;
    // const int cz = grid.cellCountZ / 2;
    // grid.AddSolidBox(cx - 4, cy - 4, cz - 4, 8, 8, 8);

    //grid.save_ppm_slice("../../3D/outputs/smoke_slice.ppm");
    return grid;
}
