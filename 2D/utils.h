#ifndef SMOKE_SIMULATOR_UTILS_H
#define SMOKE_SIMULATOR_UTILS_H

#include <vector>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include <array>

#include "color.hh"


namespace smoke {

    class FluidGrid
    {
    public:
        int cellCountX;
        int cellCountY;
        float cellSize;
        std::vector<std::vector<float>> velocitiesX;
        std::vector<std::vector<float>> velocitiesY;
        std::vector<std::vector<float>> pressureMap;

        // map of density with RGB color
        std::vector<std::vector<Color>> densityMap;

        // solid cell map
        std::vector<std::vector<bool>> solidCellMap;

        const float timeStep = 1 / 7.f;
        const float density = 1;

        FluidGrid(int _cellCountX, int _cellCountY, float _cellSize)
        : cellCountX(_cellCountX), cellCountY(_cellCountY), cellSize(_cellSize),
            velocitiesX(_cellCountX+1, std::vector<float>(_cellCountY, 0.f)),
            velocitiesY(_cellCountX, std::vector<float>(_cellCountY+1, 0.f)),
            pressureMap(_cellCountX, std::vector<float>(_cellCountY, 0.f)),
            densityMap(_cellCountX, std::vector<Color>(_cellCountY, Color(0.f, 0.f, 0.f))),
            solidCellMap(_cellCountX, std::vector<bool>(_cellCountY, false))
        {}

        float CalculateVelocityDivergenceAtCell(int cellX, int cellY);
        std::vector<std::vector<float>> CalculateDivergenceMap();
        float GetPressure(int x, int y);
        float PressureSolveCell(int x, int y);
        void SolvePressure();
        void UpdateVelocities();

        void ApplyBuoyancy(float buoyancy = 0.5f);
        void AdvectVelocity();
        void AdvectDensity();
        void DissipeDensity(float rate = 0.995f);
        void DissipeVelocity(float rate = 0.995f);

        bool IsSolid(int x, int y);
        void InitSolidBorders();
        void AddSolidRect(int x, int y, int w, int h);

        float SampleBilinear2D(const std::vector<std::vector<float>>& grid2d,
                       int gridW, int gridH,
                       float px, float py);
        Color SampleBilinear2DColor(const std::vector<std::vector<Color>>& grid2d,
                       int gridW, int gridH,
                       float px, float py);
        std::array<float,2> GetVelocityAtWorldPos(std::array<float,2> worldPos);
        void AddTurbulence(float strength, int frame);

        void save_ppm(const std::string& path, int scale = 1);
    };
}

#endif //SMOKE_SIMULATOR_UTILS_H
