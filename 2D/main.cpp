#include <iostream>
#include <string>

#include "renderSFML.h"
#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef SMOKE_ASSETS_DIR
#define SMOKE_ASSETS_DIR "assets"
#endif

std::vector<std::vector<smoke::Color>> loadImageAsColor(const std::string& path) {
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 3);

    if (!data)
        throw std::runtime_error("Impossible de charger l'image : " + path);

    std::vector result(height, std::vector<smoke::Color>(width));
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 3;
            result[y][x] = smoke::Color(
                data[idx] / 255.0f,
                data[idx + 1] / 255.0f,
                data[idx + 2] / 255.0f
            );
        }

    stbi_image_free(data);
    return result;
}

static void setupGrid(smoke::FluidGrid& grid) {
    grid.InitSolidBorders();
}

static void printUsage(const char* prog) {
    std::cout
        << "Usage: " << prog << " [options]\n\n"
        << "Without option: grayscale smoke, drawn with the mouse.\n\n"
        << "Options:\n"
        << "  --image [PATH]   turn a color image into smoke (default: assets/ladybug.png)\n"
        << "  --help           show this help\n\n"
        << "Controls: left click + drag = push the smoke, right click = add smoke\n"
        << "          D = density, P = pressure, V = velocities, Esc = quit\n";
}

int main(int argc, char* argv[]) {
    using namespace smoke;

    bool imageMode = false;
    std::string imagePath = std::string(SMOKE_ASSETS_DIR) + "/ladybug.png";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        try {
            if (arg == "--help") { printUsage(argv[0]); return 0; }
            else if (arg == "--image") {
                imageMode = true;
                // the path is optional: only take the next argument if it is not an option
                if (i + 1 < argc && argv[i + 1][0] != '-')
                    imagePath = argv[++i];
            }
            else throw std::runtime_error("Unknown argument: " + arg);
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    const int PRESSURE_ITERS = 10;
    const float BUOYANCY = 1.f;

    std::unique_ptr<FluidGrid> gridPtr;
    if (imageMode) {
        std::vector<std::vector<Color>> img;
        try {
            img = loadImageAsColor(imagePath);
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
            return 1;
        }
        gridPtr = std::make_unique<FluidGrid>(img[0].size(), img.size(), 1.f);
        for (int y = 0; y < (int)img.size(); y++)
            for (int x = 0; x < (int)img[y].size(); x++)
                gridPtr->densityMap[x][gridPtr->cellCountY - 1 - y] = img[y][x];
    } else {
        gridPtr = std::make_unique<FluidGrid>(160, 120, 1.f);
    }
    FluidGrid& grid = *gridPtr;
    setupGrid(grid);

    auto step = [&]() {
        grid.AdvectVelocity();
        if (!imageMode)
            grid.ApplyBuoyancy(BUOYANCY);
        for (int i = 0; i < PRESSURE_ITERS; i++)
            grid.SolvePressure();
        grid.UpdateVelocities();
        grid.AdvectDensity();
        grid.DissipeDensity(imageMode ? 0.999f : 0.998f);
        grid.DissipeVelocity(0.999f);
    };

    renderSFML(grid, step, imageMode);

    return 0;
}
