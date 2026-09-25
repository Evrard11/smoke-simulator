//
// Created by Evrard on 08/04/2026.
//

#ifndef SMOKE_SIMULATOR_FLUIDGRID_H
#define SMOKE_SIMULATOR_FLUIDGRID_H

#include <vector>
#include <fstream>
#include <array>
#include <string>

using namespace std;
namespace smoke {

    class FluidGrid3D
    {
    public:
        int cellCountX;
        int cellCountY;
        int cellCountZ;
        float cellSize;

        vector<vector<vector<float>>> velocitiesX;  // X+1 * Y * Z
        vector<vector<vector<float>>> velocitiesY;  // X * Y+1 * Z
        vector<vector<vector<float>>> velocitiesZ;  // X * Y * Z+1

        vector<vector<vector<float>>> pressureMap, densityMap;  // X * Y * Z
        vector<vector<vector<bool>>>  solidCellMap; // X * Y * Z

        const float timeStep = 1.f / 7.f;
        const float density  = 1.f;

        // Constructor
        FluidGrid3D(int _cellCountX, int _cellCountY, int _cellCountZ, float _cellSize);

        // Divergence
        float CalculateVelocityDivergenceAtCell(int x, int y, int z);
        vector<vector<vector<float>>> CalculateDivergenceMap();

        // Pressure solver (Jacobi)
        float GetPressure(int x, int y, int z) const;
        float PressureSolveCell(int x, int y, int z);
        void  SolvePressure();

        // Velocity projection
        void UpdateVelocities();

        // Physics
        void ApplyBuoyancy(float buoyancy = 0.5f);
        void AdvectVelocity();
        void AdvectDensity();
        void DissipeDensity(float rate = 0.995f);

        // Solid geometry
        bool IsSolid(int x, int y, int z) const;
        void InitSolidBorders();
        void AddSolidBox(int x, int y, int z, int w, int h, int d);

        // Interpolation / sampling
        float SampleTrilinear(const vector<vector<vector<float>>>& grid,
                              int gW, int gH, int gD,
                              float px, float py, float pz) const;
        array<float, 3> GetVelocityAtWorldPos(array<float, 3> worldPos) const;

        // Output
        // Saves a Z-slice of the density field as a grayscale PPM image
        void save_ppm_slice(const string& path, int sliceZ = -1) const;
    };

} // namespace smoke

#endif //SMOKE_SIMULATOR_FLUIDGRID_H
