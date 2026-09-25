#include "renderSFML.h"

#include <iostream>

float renderCellSize;

// Returns the maximum absolute value in a 2D field
// Used to normalize values for visualization
float normalizeByMax(std::vector<std::vector<float>>& vec) {
    float absMax = 0.f;
    for (const auto& row : vec) {
        for (float elt : row) {
            absMax = std::max(absMax, std::fabs(elt));
        }
    }
    return absMax > 1e-6f ? absMax : 1.0f;
}

// Overload for 3D vector fields
float normalizeByMax(std::vector<std::vector<std::vector<float>>>& vec) {
    float absMax = 0.f;
    for (auto& v : vec) {
        absMax = std::max(absMax, normalizeByMax(v));
    }
    return absMax;
}

void drawGrid(sf::RenderWindow& window, smoke::FluidGrid& grid) {
    sf::RectangleShape outline(sf::Vector2f(
        grid.cellCountX * renderCellSize + 1.f,
        grid.cellCountY * renderCellSize + 1.f));
    outline.setPosition(sf::Vector2f(renderCellSize, renderCellSize));
    outline.setFillColor(sf::Color(30, 30, 30));
    window.draw(outline);
 
    for (int x = 0; x < grid.cellCountX; x++) {
        for (int y = 0; y < grid.cellCountY; y++) {
            sf::RectangleShape square(sf::Vector2f(renderCellSize - 1.f, renderCellSize - 1.f));
            square.setPosition(sf::Vector2f(
                (x + 1) * renderCellSize + 1.f,
                (y + 1) * renderCellSize + 1.f));
            square.setFillColor(sf::Color::Black);
            window.draw(square);
        }
    }
}

// Renders colored smoke density as RGB
// Each cell displays the color stored in densityMap[x][y]
void drawDensity(sf::RenderWindow& window, smoke::FluidGrid& grid) {
    for (int x = 0; x < grid.cellCountX; x++) {
        for (int y = 0; y < grid.cellCountY; y++) {
            smoke::Color c = grid.densityMap[x][y].clamped();
            uint8_t r = static_cast<uint8_t>(c.r * 255);
            uint8_t g = static_cast<uint8_t>(c.g * 255);
            uint8_t b = static_cast<uint8_t>(c.b * 255);
 
            sf::RectangleShape square(sf::Vector2f(renderCellSize, renderCellSize));
            square.setPosition(sf::Vector2f(
                (x + 1) * renderCellSize,
                (grid.cellCountY - y) * renderCellSize));
            square.setFillColor(sf::Color(r, g, b));
            window.draw(square);
        }
    }
     for (int x = 0; x < grid.cellCountX; x++) {
        for (int y = 0; y < grid.cellCountY; y++) {
            if (!grid.IsSolid(x, y)) continue;
            sf::RectangleShape square(sf::Vector2f(renderCellSize, renderCellSize));
            square.setPosition(sf::Vector2f(
                (x + 1) * renderCellSize,
                (grid.cellCountY - y) * renderCellSize));
            square.setFillColor(sf::Color(60, 60, 60));
            window.draw(square);
        }
    }
}

// Visualizes velocity field using line segments
// X velocities and Y velocities are drawn separately
void drawVelocities(sf::RenderWindow& window, smoke::FluidGrid& grid,
                    float pMaxVeloX, float pMaxVeloY)
{
    if (pMaxVeloX == 0.f) pMaxVeloX = normalizeByMax(grid.velocitiesX);
    for (int i = 0; i < grid.cellCountX + 1; i++) {
        for (int j = 0; j < grid.cellCountY; j++) {
            float yCoord = (grid.cellCountY - j - 0.5f) * renderCellSize + renderCellSize;
            float nv = grid.velocitiesX[i][j] / pMaxVeloX;
            sf::Vertex line[] = {
                sf::Vertex{sf::Vector2f(renderCellSize + i * renderCellSize, yCoord)},
                sf::Vertex{sf::Vector2f(renderCellSize + i * renderCellSize + nv * renderCellSize / 3, yCoord)}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }
    }
 
    if (pMaxVeloY == 0.f) pMaxVeloY = normalizeByMax(grid.velocitiesY);
    for (int i = 0; i < grid.cellCountX; i++) {
        for (int j = 0; j < grid.cellCountY + 1; j++) {
            float xCoord = (i + 1.5f) * renderCellSize;
            float nv = grid.velocitiesY[i][j] / pMaxVeloY;
            float yBase = (grid.cellCountY - j) * renderCellSize + renderCellSize;
            sf::Vertex line[] = {
                sf::Vertex{sf::Vector2f(xCoord, yBase)},
                sf::Vertex{sf::Vector2f(xCoord, yBase - nv * renderCellSize / 3)}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }
    }
}

// Visualizes divergence:
// Red = negative divergence (compression)
// Blue = positive divergence (expansion)
void drawDivergence(sf::RenderWindow& window, smoke::FluidGrid& grid,
                    std::vector<std::vector<float>>& divergenceMap,
                    const sf::Font& font, float pMax)
{
    if (pMax == 0.f) pMax = normalizeByMax(divergenceMap);
    for (int x = 0; x < grid.cellCountX; x++) {
        for (int y = 0; y < grid.cellCountY; y++) {
            sf::RectangleShape square(sf::Vector2f(renderCellSize - 1, renderCellSize - 1));
            square.setPosition(sf::Vector2f(
                (x + 1) * renderCellSize + 1,
                (grid.cellCountY - y) * renderCellSize + 1));
            float nd = divergenceMap[x][y] / pMax;
            if (std::fabs(nd) < 1e-4f)
                square.setFillColor(sf::Color::Black);
            else if (nd < 0)
                square.setFillColor(sf::Color((uint8_t)(-nd * 255), 0, 0));
            else
                square.setFillColor(sf::Color(0, 0, (uint8_t)(nd * 255)));
            window.draw(square);
        }
    }
}

// Visualizes pressure field:
// Green = positive pressure
// Purple = negative pressure
void drawPressure(sf::RenderWindow& window, smoke::FluidGrid& grid,
                  const sf::Font& font, float pMax)
{
    if (pMax == 0.f) pMax = normalizeByMax(grid.pressureMap);
    for (int x = 0; x < grid.cellCountX; x++) {
        for (int y = 0; y < grid.cellCountY; y++) {
            sf::RectangleShape square(sf::Vector2f(renderCellSize - 1, renderCellSize - 1));
            square.setPosition(sf::Vector2f(
                (x + 1) * renderCellSize + 1,
                (grid.cellCountY - y) * renderCellSize + 1));
            float np = grid.pressureMap[x][y] / pMax;
            if (std::fabs(np) < 1e-4f)
                square.setFillColor(sf::Color::Black);
            else if (np < 0)
                square.setFillColor(sf::Color((uint8_t)(-np * 255), 0, (uint8_t)(-np * 255)));
            else
                square.setFillColor(sf::Color(0, (uint8_t)(np * 255), 0));
            window.draw(square);
        }
    }
}

// Displays velocity vectors sampled inside each cell
// Used to visualize continuous velocity field (not just grid edges)
void drawCells(sf::RenderWindow& window, smoke::FluidGrid& grid) {
    float cellsCount = 4.f;
    for (int x = 0; x < grid.cellCountX; x++) {
        for (int y = 0; y < grid.cellCountY; y++) {
            if (grid.IsSolid(x, y)) continue;
            for (int i = 0; i < (int)cellsCount; i++) {
                for (int j = 0; j < (int)cellsCount; j++) {
                    float offsetX = 1.f / (cellsCount + 1) * (i + 1);
                    float offsetY = 1.f / (cellsCount + 1) * (j + 1);
                    auto dirVect = grid.GetVelocityAtWorldPos({x + offsetX, y + offsetY});
                    dirVect[0] *= 10; dirVect[1] *= 10;
                    float renderX = x + offsetX;
                    float renderY = y + offsetY;
                    sf::Vertex line[] = {
                        sf::Vertex{sf::Vector2f(
                            (renderX + 1) * renderCellSize,
                            (grid.cellCountY - renderY) * renderCellSize + renderCellSize)},
                        sf::Vertex{sf::Vector2f(
                            (renderX + 1) * renderCellSize + dirVect[0],
                            (grid.cellCountY - renderY) * renderCellSize + renderCellSize - dirVect[1])}
                    };
                    line[0].color = sf::Color::Yellow;
                    line[1].color = sf::Color::Yellow;
                    window.draw(line, 2, sf::PrimitiveType::Lines);
                }
            }
        }
    }
}


// Real-time main loop
// Keys:
// D = Density mode (default)
// P = Pressure mode
// V = Overlapping velocities mode
// Esc = Exit
void renderSFML(smoke::FluidGrid& grid,
                std::function<void()> stepFn,
                bool coloredBrush)
{
    //const unsigned int winW = static_cast<unsigned int>((grid.cellCountX + 2) * 8);
    //const unsigned int winH = static_cast<unsigned int>((grid.cellCountY + 2) * 8);
 
    // Calculating renderCellSize to fill the screen
    auto desktopMode = sf::VideoMode::getDesktopMode();
    renderCellSize = std::min(
        (float)desktopMode.size.x / (grid.cellCountX + 2),
        (float)desktopMode.size.y / (grid.cellCountY + 2)) - 1.f;
    renderCellSize = std::max(renderCellSize, 1.f);
 
    sf::RenderWindow window(sf::VideoMode({
        static_cast<unsigned int>((grid.cellCountX + 2) * renderCellSize),
        static_cast<unsigned int>((grid.cellCountY + 2) * renderCellSize)
    }), "Smoke Simulator  [D=Density  P=Pressure  V=Velocity  Esc=Quit]");
 
    window.setFramerateLimit(60);
 
    sf::Font font;
 
    enum class DisplayMode { Density, Pressure, Velocity };
    DisplayMode mode = DisplayMode::Density;
 
    sf::Clock clock;
 
    while (window.isOpen()) {
        // Events
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return;
            }
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                switch (key->code) {
                    case sf::Keyboard::Key::Escape: window.close(); return;
                    case sf::Keyboard::Key::D: mode = DisplayMode::Density;  break;
                    case sf::Keyboard::Key::P: mode = DisplayMode::Pressure; break;
                    case sf::Keyboard::Key::V: mode = DisplayMode::Velocity; break;
                    default: break;
                }
            }
            // Mouse control
            auto mousePos = sf::Mouse::getPosition(window);
            auto windowMousePos = window.mapPixelToCoords(mousePos);

            int gridX = static_cast<int>(windowMousePos.x / renderCellSize) - 1;
            int gridY = static_cast<int>((window.getSize().y - windowMousePos.y) / renderCellSize) - 1;

            if (gridX >= 1 && gridX < grid.cellCountX - 1 &&
                gridY >= 1 && gridY < grid.cellCountY - 1)
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
                {
                    static auto lastMousePos = windowMousePos;
                    static float velXSmoothed = 0.f, velYSmoothed = 0.f;
                    const float smoothing = 0.9f;

                    float velX = (windowMousePos.x - lastMousePos.x) * 0.1f;
                    float velY = -(windowMousePos.y - lastMousePos.y) * 0.1f;
                    velXSmoothed = velXSmoothed * (1 - smoothing) + velX * smoothing;
                    velYSmoothed = velYSmoothed * (1 - smoothing) + velY * smoothing;

                    // apply to radius
                    int radius = 10;
                    for (int dx = -radius; dx <= radius; dx++) {
                        for (int dy = -radius; dy <= radius; dy++) {
                            if (dx * dx + dy * dy >= radius * radius) continue;
                            int cx = gridX + dx;
                            int cy = gridY + dy;
                            if (cx >= 1 && cx < grid.cellCountX - 1 && cy >= 1 && cy < grid.cellCountY - 1) {
                                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                                    grid.velocitiesX[cx][cy] += velXSmoothed;
                                    grid.velocitiesY[cx][cy] += velYSmoothed;
                                } else {
                                    // right click inject smoke: white, or colored depending on X position
                                    float r = 1.f, g = 1.f, b = 1.f;
                                    if (coloredBrush) {
                                        float hue = static_cast<float>(cx) / grid.cellCountX * 6.f;
                                        r = std::max(0.f, std::sin(hue));
                                        g = std::max(0.f, std::sin(hue + 2.f));
                                        b = std::max(0.f, std::sin(hue + 4.f));
                                    }
                                    grid.densityMap[cx][cy] = smoke::Color(
                                        std::min(grid.densityMap[cx][cy].r + r * 0.5f, 1.f),
                                        std::min(grid.densityMap[cx][cy].g + g * 0.5f, 1.f),
                                        std::min(grid.densityMap[cx][cy].b + b * 0.5f, 1.f)
                                    );
                                }
                            }
                        }
                    }
                    lastMousePos = windowMousePos;
                }
            }
        }
 
        // Advance the simulation
        stepFn();
 
        // rendered
        window.clear(sf::Color::Black);
 
        switch (mode) {
            case DisplayMode::Density:
                drawDensity(window, grid);
                break;
 
            case DisplayMode::Pressure: {
                drawGrid(window, grid);
                drawPressure(window, grid, font);
                break;
            }
 
            case DisplayMode::Velocity:
                drawDensity(window, grid);
                drawVelocities(window, grid);
                break;
        }
 
        window.display();
    }
}
