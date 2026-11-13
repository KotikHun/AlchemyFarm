#include "WeatherSystem.hpp"
#include <cstdlib>

WeatherSystem::WeatherSystem()
{
    init();
}

void WeatherSystem::init()
{
    rainDrops.clear();
    for (int i = 0; i < 110; i++)
    {
        RainDrop d;
        d.pos = { static_cast<float>(rand() % 980), static_cast<float>(rand() % 560) };
        d.speed = 460.f + (rand() % 160);
        d.length = 12.f + (rand() % 8);
        rainDrops.push_back(d);
    }
}

void WeatherSystem::update(float dt, bool isRaining)
{
    if (!isRaining) return;

    for (auto& d : rainDrops)
    {
        d.pos.y += d.speed * dt;
        d.pos.x -= d.speed * 0.22f * dt;

        if (d.pos.y > 550.f || d.pos.x < -20.f)
        {
            d.pos.y = -15.f - (rand() % 30);
            d.pos.x = static_cast<float>(rand() % 1020);
            d.speed = 460.f + (rand() % 160);
            d.length = 12.f + (rand() % 8);
        }
    }
}

void WeatherSystem::draw(sf::RenderWindow& window, bool isRaining, bool isInside, float virtualWidth, float virtualHeight)
{
    if (!isRaining || isInside) return;

    // Легкая серовато-голубая пелена дождя
    sf::RectangleShape rainMist({ virtualWidth, virtualHeight });
    rainMist.setFillColor(sf::Color(20, 35, 55, 45));
    window.draw(rainMist);

    // Капли дождя
    for (const auto& d : rainDrops)
    {
        sf::Vertex line[2] = {
            sf::Vertex(d.pos, sf::Color(170, 210, 255, 200)),
            sf::Vertex({ d.pos.x - d.length * 0.22f, d.pos.y + d.length }, sf::Color(220, 240, 255, 240))
        };
        window.draw(line, 2, sf::PrimitiveType::Lines);

        // Вторая параллельная линия со смещением 1px для плотности и четкости капель
        sf::Vertex line2[2] = {
            sf::Vertex({ d.pos.x + 1.f, d.pos.y }, sf::Color(160, 200, 255, 120)),
            sf::Vertex({ d.pos.x + 1.f - d.length * 0.22f, d.pos.y + d.length }, sf::Color(200, 230, 255, 150))
        };
        window.draw(line2, 2, sf::PrimitiveType::Lines);
    }
}
