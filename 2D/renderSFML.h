#ifndef SMOKE_SIMULATOR_RENDERSFML_H
#define SMOKE_SIMULATOR_RENDERSFML_H
#include <SFML/Graphics.hpp>
#include <functional>
#include <thread>

#include "utils.h"

void drawGrid(sf::RenderWindow& window, smoke::FluidGrid& grid);

void drawVelocities(sf::RenderWindow& window, smoke::FluidGrid& grid, float pMaxVeloX=0.f, float pMaxVeloY=0.f);

void drawDivergence(sf::RenderWindow& window, smoke::FluidGrid& grid,
    std::vector<float>& divergenceMap, const sf::Font& font, float pMax=0.f);

void drawPressure(sf::RenderWindow& window, smoke::FluidGrid& grid, const sf::Font& font, float pMax=0.f);

void drawCells(sf::RenderWindow& window, smoke::FluidGrid& grid);

// coloredBrush: right click adds colored smoke instead of white smoke
void renderSFML(smoke::FluidGrid& grid, std::function<void()> stepFn,
    bool coloredBrush = false);


#endif //SMOKE_SIMULATOR_RENDERSFML_H
