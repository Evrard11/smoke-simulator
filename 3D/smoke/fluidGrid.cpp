//
// Created by Evrard on 08/04/2026.
//

#include "fluidGrid.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace smoke {

// Constructor
FluidGrid3D::FluidGrid3D(int _cellCountX, int _cellCountY, int _cellCountZ, float _cellSize)
    : cellCountX(_cellCountX), cellCountY(_cellCountY), cellCountZ(_cellCountZ), cellSize(_cellSize),
      velocitiesX(_cellCountX + 1, vector(_cellCountY, vector(_cellCountZ, 0.f))),
      velocitiesY(_cellCountX, vector(_cellCountY + 1, vector(_cellCountZ, 0.f))),
      velocitiesZ(_cellCountX, vector(_cellCountY, vector(_cellCountZ + 1, 0.f))),
      pressureMap(_cellCountX, vector(_cellCountY, vector(_cellCountZ, 0.f))),
      densityMap(_cellCountX, vector(_cellCountY, vector(_cellCountZ, 0.f))),
      solidCellMap(_cellCountX, vector(_cellCountY, vector(_cellCountZ, false)))
{}


// Divergence

float FluidGrid3D::CalculateVelocityDivergenceAtCell(const int x, const int y, const int z) {
    const float dvx = (velocitiesX[x + 1][y][z] - velocitiesX[x][y][z]) / cellSize;
    const float dvy = (velocitiesY[x][y + 1][z] - velocitiesY[x][y][z]) / cellSize;
    const float dvz = (velocitiesZ[x][y][z + 1] - velocitiesZ[x][y][z]) / cellSize;
    return dvx + dvy + dvz;
}

vector<vector<vector<float>>> FluidGrid3D::CalculateDivergenceMap() {
    vector<vector<vector<float>>> divergenceMap = vector(cellCountX, vector(cellCountY, vector(cellCountZ, 0.f)));
    for (int x = 0; x < cellCountX; x++)
        for (int y = 0; y < cellCountY; y++)
            for (int z = 0; z < cellCountZ; z++)
                divergenceMap[x][y][z] = CalculateVelocityDivergenceAtCell(x, y, z);
    return divergenceMap;
}

// Pressure solver (Jacobi)

float FluidGrid3D::GetPressure(const int x, const int y, const int z) const {
    if (x < 0 || x >= cellCountX || y < 0 || y >= cellCountY || z < 0 || z >= cellCountZ) return 0.f;
    return pressureMap[x][y][z];
}

float FluidGrid3D::PressureSolveCell(const int x, const int y, const int z) {
    // Flow flags for each of the 6 neighbors face
    const int fRight = IsSolid(x + 1, y, z) ? 0 : 1;
    const int fLeft = IsSolid(x - 1, y, z) ? 0 : 1;
    const int fTop = IsSolid(x, y + 1, z) ? 0 : 1;
    const int fBottom = IsSolid(x, y - 1, z) ? 0 : 1;
    const int fFront = IsSolid(x, y, z + 1) ? 0 : 1;
    const int fBack = IsSolid(x, y, z - 1) ? 0 : 1;

    const int fluidFaceCount = fRight + fLeft + fTop + fBottom + fFront + fBack;

    if (IsSolid(x, y, z) || fluidFaceCount == 0) return 0.f;

    // Neighbor pressures (zero at solid boundaries)
    const float pressureRight = GetPressure(x + 1, y, z) * fRight;
    const float pressureLeft = GetPressure(x - 1, y, z) * fLeft;
    const float pressureTop = GetPressure(x, y + 1, z) * fTop;
    const float pressureBottom = GetPressure(x, y - 1, z) * fBottom;
    const float pressureFront = GetPressure(x, y, z + 1) * fFront;
    const float pressureBack= GetPressure(x, y, z - 1) * fBack;

    // Face velocities (zero at solid boundaries)
    const float vxRight = velocitiesX[x + 1][y][z] * fRight;
    const float vxLeft = velocitiesX[x][y][z] * fLeft;
    const float vyTop = velocitiesY[x][y + 1][z] * fTop;
    const float vyBottom = velocitiesY[x][y][z] * fBottom;
    const float vzFront = velocitiesZ[x][y][z + 1] * fFront;
    const float vzBack = velocitiesZ[x][y][z] * fBack;

    const float pressureSum  = pressureRight + pressureLeft + pressureTop + pressureBottom + pressureFront + pressureBack;
    const float deltaVelocity = (vxRight - vxLeft) + (vyTop - vyBottom) + (vzFront - vzBack);
    return (pressureSum - density * cellSize * deltaVelocity / timeStep) / fluidFaceCount;
}

void FluidGrid3D::SolvePressure() {
    for (int x = 0; x < cellCountX; x++)
        for (int y = 0; y < cellCountY; y++)
            for (int z = 0; z < cellCountZ; z++)
                pressureMap[x][y][z] = PressureSolveCell(x, y, z);
}

// Velocity projection

void FluidGrid3D::UpdateVelocities() {
    const float K = timeStep / (density * cellSize);
    // X faces
    for (int x = 0; x <= cellCountX; x++)
        for (int y = 0; y < cellCountY; y++)
            for (int z = 0; z < cellCountZ; z++) {
                if (IsSolid(x, y, z) || IsSolid(x - 1, y, z)) {
                    velocitiesX[x][y][z] = 0.f;
                    continue;
                }
                velocitiesX[x][y][z] -= K * (GetPressure(x, y, z) - GetPressure(x - 1, y, z));
            }
    // Y faces
    for (int x = 0; x < cellCountX; x++)
        for (int y = 0; y <= cellCountY; y++)
            for (int z = 0; z < cellCountZ; z++) {
                if (IsSolid(x, y, z) || IsSolid(x, y - 1, z)) {
                    velocitiesY[x][y][z] = 0.f;
                    continue;
                }
                velocitiesY[x][y][z] -= K * (GetPressure(x, y, z) - GetPressure(x, y - 1, z));
            }
    // Z faces
    for (int x = 0; x < cellCountX; x++)
        for (int y = 0; y < cellCountY; y++)
            for (int z = 0; z <= cellCountZ; z++) {
                if (IsSolid(x, y, z) || IsSolid(x, y, z - 1)) {
                    velocitiesZ[x][y][z] = 0.f;
                    continue;
                }
                velocitiesZ[x][y][z] -= K * (GetPressure(x, y, z) - GetPressure(x, y, z - 1));
            }
}

// Physics

void FluidGrid3D::ApplyBuoyancy(const float buoyancy) {
    for (int x = 0; x < cellCountX; x++)
        for (int y = 1; y <= cellCountY; y++)
            for (int z = 0; z < cellCountZ; z++) {
                if (IsSolid(x, y, z) || IsSolid(x, y - 1, z)) continue;
                const float avgDensity =
                    (densityMap[x][y < cellCountY ? y : cellCountY - 1][z] +
                     densityMap[x][y - 1][z]) * 0.5f;
                velocitiesY[x][y][z] += buoyancy * avgDensity * timeStep;
            }
}

void FluidGrid3D::AdvectVelocity() {
    auto newVelX = velocitiesX;
    for (int i = 0; i <= cellCountX; i++)
        for (int j = 0; j < cellCountY; j++)
            for (int k = 0; k < cellCountZ; k++) {
                if (IsSolid(i, j, k) || IsSolid(i - 1, j, k)) continue;
                auto [vx, vy, vz] = GetVelocityAtWorldPos({(float)i, j + 0.5f, k + 0.5f});
                float px = clamp(i - vx * timeStep, 0.f, (float)cellCountX);
                float py = clamp(j - vy * timeStep, 0.f, (float)(cellCountY - 1));
                float pz = clamp(k - vz * timeStep, 0.f, (float)(cellCountZ - 1));
                newVelX[i][j][k] = SampleTrilinear(velocitiesX, cellCountX + 1, cellCountY, cellCountZ, px, py, pz);
            }
    velocitiesX = std::move(newVelX);

    auto newVelY = velocitiesY;
    for (int i = 0; i < cellCountX; i++)
        for (int j = 0; j <= cellCountY; j++)
            for (int k = 0; k < cellCountZ; k++) {
                if (IsSolid(i, j, k) || IsSolid(i, j - 1, k)) continue;
                auto [vx, vy, vz] = GetVelocityAtWorldPos({i + 0.5f, (float)j, k + 0.5f});
                float px = clamp(i - vx * timeStep, 0.f, (float)(cellCountX - 1));
                float py = clamp(j - vy * timeStep, 0.f, (float)cellCountY);
                float pz = clamp(k - vz * timeStep, 0.f, (float)(cellCountZ - 1));
                newVelY[i][j][k] = SampleTrilinear(velocitiesY, cellCountX, cellCountY + 1, cellCountZ, px, py, pz);
            }
    velocitiesY = std::move(newVelY);

    auto newVelZ = velocitiesZ;
    for (int i = 0; i < cellCountX; i++)
        for (int j = 0; j < cellCountY; j++)
            for (int k = 0; k <= cellCountZ; k++) {
                if (IsSolid(i, j, k) || IsSolid(i, j, k - 1)) continue;
                auto [vx, vy, vz] = GetVelocityAtWorldPos({i + 0.5f, j + 0.5f, (float)k});
                float px = clamp(i - vx * timeStep, 0.f, (float)(cellCountX - 1));
                float py = clamp(j - vy * timeStep, 0.f, (float)(cellCountY - 1));
                float pz = clamp(k - vz * timeStep, 0.f, (float)cellCountZ);
                newVelZ[i][j][k] = SampleTrilinear(velocitiesZ, cellCountX, cellCountY, cellCountZ + 1, px, py, pz);
            }
    velocitiesZ = std::move(newVelZ);
}


void FluidGrid3D::AdvectDensity() {
    vector<vector<vector<float>>> newDensity = vector(cellCountX, vector(cellCountY, vector(cellCountZ, 0.f)));
    for (int x = 0; x < cellCountX; x++)
        for (int y = 0; y < cellCountY; y++)
            for (int z = 0; z < cellCountZ; z++) {
                if (IsSolid(x, y, z)) continue;

                const float wx = x + 0.5f;
                const float wy = y + 0.5f;
                const float wz = z + 0.5f;

                auto [vx, vy, vz] = GetVelocityAtWorldPos({wx, wy, wz});

                const float px = clamp(wx - vx * timeStep - 0.5f, 0.f, (float)(cellCountX - 1));
                const float py = clamp(wy - vy * timeStep - 0.5f, 0.f, (float)(cellCountY - 1));
                const float pz = clamp(wz - vz * timeStep - 0.5f, 0.f, (float)(cellCountZ - 1));

                newDensity[x][y][z] = SampleTrilinear(densityMap, cellCountX, cellCountY, cellCountZ, px, py, pz);
            }
    densityMap = std::move(newDensity);
}

void FluidGrid3D::DissipeDensity(const float rate) {
    for (int x = 0; x < cellCountX; x++)
        for (int y = 0; y < cellCountY; y++)
            for (int z = 0; z < cellCountZ; z++)
                densityMap[x][y][z] *= rate;
}

// Solid geometry

bool FluidGrid3D::IsSolid(const int x, const int y, const int z) const {
    if (x < 0 || x >= cellCountX || y < 0 || y >= cellCountY || z < 0 || z >= cellCountZ) return true;
    return solidCellMap[x][y][z];
}

void FluidGrid3D::InitSolidBorders() {
    for (int x = 0; x < cellCountX; x++)
        for (int y = 0; y < cellCountY; y++) {
            solidCellMap[x][y][0] = true;
            solidCellMap[x][y][cellCountZ - 1] = true;
        }
    for (int x = 0; x < cellCountX; x++)
        for (int z = 0; z < cellCountZ; z++) {
            solidCellMap[x][0][z] = true;
            solidCellMap[x][cellCountY - 1][z] = true;
        }
    for (int y = 0; y < cellCountY; y++)
        for (int z = 0; z < cellCountZ; z++) {
            solidCellMap[0][y][z] = true;
            solidCellMap[cellCountX - 1][y][z] = true;
        }
}

void FluidGrid3D::AddSolidBox(const int x, const int y, const int z,
    const int w, const int h, const int d)
{
    for (int i = x; i < x + w && i < cellCountX; i++)
        for (int j = y; j < y + h && j < cellCountY; j++)
            for (int k = z; k < z + d && k < cellCountZ; k++)
                solidCellMap[i][j][k] = true;
}

// Interpolation

float FluidGrid3D::SampleTrilinear(const vector<vector<vector<float>>>& grid, const int gW, const int gH, const int gD,
    const float px, const float py, const float pz) const
{
    const int x0 = clamp((int)px, 0, gW - 2);
    const int y0 = clamp((int)py, 0, gH - 2);
    const int z0 = clamp((int)pz, 0, gD - 2);
    const int x1 = x0 + 1;
    const int y1 = y0 + 1;
    const int z1 = z0 + 1;

    const float tx = clamp(px - x0, 0.f, 1.f);
    const float ty = clamp(py - y0, 0.f, 1.f);
    const float tz = clamp(pz - z0, 0.f, 1.f);

    // Interpolate along X
    const float c00 = lerp(grid[x0][y0][z0], grid[x1][y0][z0], tx);
    const float c10 = lerp(grid[x0][y1][z0], grid[x1][y1][z0], tx);
    const float c01 = lerp(grid[x0][y0][z1], grid[x1][y0][z1], tx);
    const float c11 = lerp(grid[x0][y1][z1], grid[x1][y1][z1], tx);
    // Interpolate along Y
    const float c0 = lerp(c00, c10, ty);
    const float c1 = lerp(c01, c11, ty);
    // Interpolate along Z
    return lerp(c0, c1, tz);
}

array<float, 3> FluidGrid3D::GetVelocityAtWorldPos(const array<float, 3> worldPos) const {
    const float wx = worldPos[0];
    const float wy = worldPos[1];
    const float wz = worldPos[2];

    const float velX = SampleTrilinear(
        velocitiesX, cellCountX + 1, cellCountY, cellCountZ,
        clamp(wx, 0.f, (float)cellCountX),
        clamp(wy - 0.5f, 0.f, (float)(cellCountY - 1)),
        clamp(wz - 0.5f, 0.f, (float)(cellCountZ - 1)));
    const float velY = SampleTrilinear(
        velocitiesY, cellCountX, cellCountY + 1, cellCountZ,
        clamp(wx - 0.5f, 0.f, (float)(cellCountX - 1)),
        clamp(wy, 0.f, (float)cellCountY),
        clamp(wz - 0.5f, 0.f, (float)(cellCountZ - 1)));
    const float velZ = SampleTrilinear(
        velocitiesZ, cellCountX, cellCountY, cellCountZ + 1,
        clamp(wx - 0.5f, 0.f, (float)(cellCountX - 1)),
        clamp(wy - 0.5f, 0.f, (float)(cellCountY - 1)),
        clamp(wz, 0.f, (float)cellCountZ));

    return {velX, velY, velZ};
}

// Output

void FluidGrid3D::save_ppm_slice(const string& path, int sliceZ) const {
    if (sliceZ < 0) sliceZ = cellCountZ / 2;
    sliceZ = clamp(sliceZ, 0, cellCountZ - 1);

    ofstream file(path, ios::binary);
    if (!file) throw runtime_error("Cannot open file: " + path);

    file << "P6\n" << cellCountX << " " << cellCountY << "\n255\n";
    for (int y = 0; y < cellCountY; ++y) {
        for (int x = 0; x < cellCountX; ++x) {
            const uint8_t v = static_cast<uint8_t>(
                clamp(densityMap[x][cellCountY - 1 - y][sliceZ], 0.f, 1.f) * 255.f);
            file.write(reinterpret_cast<const char*>(&v), 1);
            file.write(reinterpret_cast<const char*>(&v), 1);
            file.write(reinterpret_cast<const char*>(&v), 1);
        }
    }
}

} // namespace smoke
