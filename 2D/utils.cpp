#include "utils.h"
#include <cmath>
#include <cstdint>

namespace smoke
{
    // Computes divergence of the velocity field at a given cell
    float FluidGrid::CalculateVelocityDivergenceAtCell(const int cellX, const int cellY) {
        const float velocityTop = velocitiesY[cellX][cellY + 1];
        const float velocityLeft = velocitiesX[cellX][cellY];
        const float velocityRight = velocitiesX[cellX + 1][cellY];
        const float velocityBottom = velocitiesY[cellX][cellY];

        const float gradientX = (velocityRight - velocityLeft) / cellSize;
        const float gradientY = (velocityTop - velocityBottom) / cellSize;

        return gradientX + gradientY;
    }

    std::vector<std::vector<float>> FluidGrid::CalculateDivergenceMap() {
        std::vector<std::vector<float>> divergenceMap(cellCountX, std::vector<float>(cellCountY));
        for (int x = 0; x < cellCountX; x++)
            for (int y = 0; y < cellCountY; y++)
                divergenceMap[x][y] = CalculateVelocityDivergenceAtCell(x, y);
        return divergenceMap;
    }

    float FluidGrid::GetPressure(int x, int y) {
        bool outOfBounds = x < 0 || x >= cellCountX || y < 0 || y >= cellCountY;
        return outOfBounds ? 0.f : pressureMap[x][y];
    }

    // Computes updated pressure at a cell using a Jacobi iteration
    float FluidGrid::PressureSolveCell(int x, int y) {
        int flowTop = IsSolid(x, y + 1) ? 0 : 1;
        int flowLeft = IsSolid(x - 1, y) ? 0 : 1;
        int flowRight = IsSolid(x + 1, y) ? 0 : 1;
        int flowBottom = IsSolid(x, y - 1) ? 0 : 1;
        int fluidEdgeCount = flowTop + flowLeft + flowRight + flowBottom;

        if (IsSolid(x, y) || fluidEdgeCount == 0) return 0.f;

        float pressureTop = GetPressure(x, y + 1) * flowTop;
        float pressureLeft = GetPressure(x - 1, y) * flowLeft;
        float pressureRight = GetPressure(x + 1, y) * flowRight;
        float pressureBottom = GetPressure(x, y - 1) * flowBottom;

        float velocityTop = velocitiesY[x][y + 1] * flowTop;
        float velocityLeft = velocitiesX[x][y] * flowLeft;
        float velocityRight = velocitiesX[x + 1][y] * flowRight;
        float velocityBottom = velocitiesY[x][y] * flowBottom;

        float pressureSum = pressureRight + pressureLeft + pressureTop + pressureBottom;
        float deltaVelocitySum = (velocityRight - velocityLeft) + (velocityTop - velocityBottom);
        return (pressureSum - density * cellSize * deltaVelocitySum / timeStep) / fluidEdgeCount;
    }

    // Performs one iteration of pressure solving over the entire grid
    // Multiple iterations are required for convergence
    void FluidGrid::SolvePressure() {
        for (int x = 0; x < cellCountX; x++)
            for (int y = 0; y < cellCountY; y++)
                pressureMap[x][y] = PressureSolveCell(x, y);
    }

    // Updates velocity field using pressure gradients
    // This step enforces incompressibility
    void FluidGrid::UpdateVelocities() {
        const float K = timeStep / (density * cellSize);

        for (int x = 0; x < cellCountX + 1; x++) {
            for (int y = 0; y < cellCountY; y++) {
                if (IsSolid(x, y) || IsSolid(x - 1, y)) {
                    velocitiesX[x][y] = 0;
                    continue;
                }
                float pressureRight = GetPressure(x, y);
                float pressureLeft = GetPressure(x - 1, y);
                velocitiesX[x][y] -= K * (pressureRight - pressureLeft);
            }
        }

        for (int x = 0; x < cellCountX; x++) {
            for (int y = 0; y < cellCountY + 1; y++) {
                if (IsSolid(x, y) || IsSolid(x, y - 1)) {
                    velocitiesY[x][y] = 0;
                    continue;
                }
                float pressureTop = GetPressure(x, y);
                float pressureBottom = GetPressure(x, y - 1);
                velocitiesY[x][y] -= K * (pressureTop - pressureBottom);
            }
        }
    }

    // Applies upward force proportional to local density
    // Simulates hot smoke rising
    // Uses red channel for buoyancy computation
    void FluidGrid::ApplyBuoyancy(float buoyancy) {
        for (int x = 0; x < cellCountX; x++) {
            for (int y = 1; y < cellCountY; y++) {
                if (IsSolid(x, y) || IsSolid(x, y - 1)) continue;
                float avgDensity = (densityMap[x][y].r + densityMap[x][y - 1].r) * 0.5f;
                velocitiesY[x][y] += buoyancy * avgDensity * timeStep;
            }
        }
    }

    // Transports velocity field using semi-Lagrangian advection (self-advection).
    // Without this, pressure-generated horizontal velocities accumulate in place
    void FluidGrid::AdvectVelocity() {
        auto newVelX = velocitiesX;
        for (int i = 0; i <= cellCountX; i++) {
            for (int j = 0; j < cellCountY; j++) {
                if (IsSolid(i, j) || IsSolid(i - 1, j)) continue;
                auto [vx, vy] = GetVelocityAtWorldPos({(float)i, j + 0.5f});
                float px = std::clamp(i - vx * timeStep, 0.f, (float)cellCountX);
                float py = std::clamp(j - vy * timeStep, 0.f, (float)(cellCountY - 1));
                newVelX[i][j] = SampleBilinear2D(velocitiesX, cellCountX + 1, cellCountY, px, py);
            }
        }
        velocitiesX = newVelX;

        auto newVelY = velocitiesY;
        for (int i = 0; i < cellCountX; i++) {
            for (int j = 0; j <= cellCountY; j++) {
                if (IsSolid(i, j) || IsSolid(i, j - 1)) continue;
                auto [vx, vy] = GetVelocityAtWorldPos({i + 0.5f, (float)j});
                float px = std::clamp(i - vx * timeStep, 0.f, (float)(cellCountX - 1));
                float py = std::clamp(j - vy * timeStep, 0.f, (float)cellCountY);
                newVelY[i][j] = SampleBilinear2D(velocitiesY, cellCountX, cellCountY + 1, px, py);
            }
        }
        velocitiesY = newVelY;
    }

    // Transports density field using semi-Lagrangian advection
    void FluidGrid::AdvectDensity() {
        std::vector<std::vector<Color>> newDensity(cellCountX, std::vector<Color>(cellCountY, Color(0.f, 0.f, 0.f)));
        for (int x = 0; x < cellCountX; x++) {
            for (int y = 0; y < cellCountY; y++) {
                if (IsSolid(x, y)) continue;
                float wx = x + 0.5f;
                float wy = y + 0.5f;
                auto [vx, vy] = GetVelocityAtWorldPos({wx, wy});
                float prevX = wx - vx * timeStep;
                float prevY = wy - vy * timeStep;
                float px = prevX - 0.5f;
                float py = prevY - 0.5f;
                newDensity[x][y] = SampleBilinear2DColor(densityMap, cellCountX, cellCountY,
                    std::clamp(px, 0.f, (float)(cellCountX - 1)),
                    std::clamp(py, 0.f, (float)(cellCountY - 1)));
            }
        }
        densityMap = newDensity;
    }

    // Applies exponential decay to density (simulates smoke fading)
    // Decays all RGB channels
    void FluidGrid::DissipeDensity(float rate) {
        for (int x = 0; x < cellCountX; x++)
            for (int y = 0; y < cellCountY; y++)
                densityMap[x][y] = densityMap[x][y] * rate;
    }

    void FluidGrid::DissipeVelocity(float rate) {
        for (int x = 0; x < cellCountX; x++)
            for (int y = 0; y < cellCountY; y++) {
                velocitiesX[x][y] = velocitiesX[x][y] * rate;
                velocitiesY[x][y] = velocitiesY[x][y] * rate;
            }
    }

    // Returns true if cell is solid or outside grid
    bool FluidGrid::IsSolid(int x, int y) {
        if (x < 0 || x >= cellCountX || y < 0 || y >= cellCountY)
            return true;
        return solidCellMap[x][y];
    }
    // Marks grid borders as solid
    void FluidGrid::InitSolidBorders() {
        for (int x = 0; x < cellCountX; x++) {
            solidCellMap[x][0] = true;
            solidCellMap[x][cellCountY - 1] = true;
        }
        for (int y = 0; y < cellCountY; y++) {
            solidCellMap[0][y] = true;
            solidCellMap[cellCountX - 1][y] = true;
        }
    }

    // Adds a rectangular solid obstacle inside the grid
    void FluidGrid::AddSolidRect(int x, int y, int w, int h) {
        for (int i = x; i < x + w && i < cellCountX; i++)
            for (int j = y; j < y + h && j < cellCountY; j++)
                solidCellMap[i][j] = true;
    }

    // Saves current colored density field as RGB PPM image
    void FluidGrid::save_ppm(const std::string& path, int scale) {
        const int outW = cellCountX * scale;
        const int outH = cellCountY * scale;

        std::ofstream file(path, std::ios::binary);
        if (!file) throw std::runtime_error("Cannot open file: " + path);

        file << "P6\n" << outW << " " << outH << "\n255\n";
        for (int j = 0; j < outH; ++j) {
            float py = std::clamp((float)(outH - 1 - j) / scale, 0.f, (float)(cellCountY - 1));
            for (int i = 0; i < outW; ++i) {
                float px = std::clamp((float)i / scale, 0.f, (float)(cellCountX - 1));
                Color c = SampleBilinear2DColor(densityMap, cellCountX, cellCountY, px, py).clamped();
                uint8_t r = static_cast<uint8_t>(c.r * 255);
                uint8_t g = static_cast<uint8_t>(c.g * 255);
                uint8_t b = static_cast<uint8_t>(c.b * 255);
                file.write(reinterpret_cast<const char*>(&r), 1);
                file.write(reinterpret_cast<const char*>(&g), 1);
                file.write(reinterpret_cast<const char*>(&b), 1);
            }
        }
    }

    // Performs bilinear interpolation on a 2D scalar field
    float FluidGrid::SampleBilinear2D(const std::vector<std::vector<float>>& grid2d, int gridW, int gridH, float px, float py) {
        int x0 = std::clamp((int)px, 0, gridW - 2);
        int y0 = std::clamp((int)py, 0, gridH - 2);
        int x1 = x0 + 1;
        int y1 = y0 + 1;

        float tx = std::clamp(px - x0, 0.f, 1.f);
        float ty = std::clamp(py - y0, 0.f, 1.f);

        float v00 = grid2d[x0][y0];
        float v10 = grid2d[x1][y0];
        float v01 = grid2d[x0][y1];
        float v11 = grid2d[x1][y1];

        float bottom = std::lerp(v00, v10, tx);
        float top = std::lerp(v01, v11, tx);
        return std::lerp(bottom, top, ty);
    }

    // Performs bilinear interpolation on a 2D color field
    Color FluidGrid::SampleBilinear2DColor(const std::vector<std::vector<Color>>& grid2d, int gridW, int gridH, float px, float py) {
        int x0 = std::clamp((int)px, 0, gridW - 2);
        int y0 = std::clamp((int)py, 0, gridH - 2);
        int x1 = x0 + 1;
        int y1 = y0 + 1;

        float tx = std::clamp(px - x0, 0.f, 1.f);
        float ty = std::clamp(py - y0, 0.f, 1.f);

        Color v00 = grid2d[x0][y0];
        Color v10 = grid2d[x1][y0];
        Color v01 = grid2d[x0][y1];
        Color v11 = grid2d[x1][y1];

        Color bottom = v00 * (1.f - tx) + v10 * tx;
        Color top = v01 * (1.f - tx) + v11 * tx;
        return bottom * (1.f - ty) + top * ty;
    }

    // Samples velocity field at arbitrary (non-grid) position
    std::array<float, 2> FluidGrid::GetVelocityAtWorldPos(std::array<float, 2> worldPos) {
        float wx = worldPos[0];
        float wy = worldPos[1];

        float pxX = wx;
        float pyX = wy - 0.5f;
        float velX = SampleBilinear2D(velocitiesX, cellCountX + 1, cellCountY,
            std::clamp(pxX, 0.f, (float)(cellCountX)),
            std::clamp(pyX, 0.f, (float)(cellCountY - 1)));

        float pxY = wx - 0.5f;
        float pyY = wy;
        float velY = SampleBilinear2D(velocitiesY, cellCountX, cellCountY + 1,
            std::clamp(pxY, 0.f, (float)(cellCountX - 1)),
            std::clamp(pyY, 0.f, (float)(cellCountY)));

        return {velX, velY};
    }

    void FluidGrid::AddTurbulence(float strength, int frame) {
        // sinus noise on red channel
        for (int x = 1; x < cellCountX - 1; x++) {
            for (int y = 10; y < cellCountY / 2; y++) {
                if (densityMap[x][y].r > 0.01f) {
                    float noise = strength * std::sin(x * 0.3f + frame * 0.05f)
                                           * std::cos(y * 0.2f + frame * 0.03f);
                    velocitiesX[x][y] += noise;
                }
            }
        }
    }

} // namespace smoke
