#include "WeatherSystem.hpp"
#include <cstdlib>

WeatherSystem::WeatherSystem()
{
    init();
}

void WeatherSystem::init()
{
    rainDrops.clear();
    for (int i = 0; i < 70; i++)
    {
        RainDrop d;
        d.pos = { static_cast<float>(rand() % 960), static_cast<float>(rand() % 800) };
        d.speed = 480.f + (rand() % 160);
        d.length = 10.f + (rand() % 8);
        rainDrops.push_back(d);
    }
}

void WeatherSystem::update(float dt, bool isRaining)
{
    if (!isRaining) return;

    for (auto& d : rainDrops)
    {
        d.pos.y += d.speed * dt;
        d.pos.x -= d.speed * 0.25f * dt;

        if (d.pos.y > 810.f || d.pos.x < -20.f)
        {
            d.pos.y = -15.f - (rand() % 30);
            d.pos.x = static_cast<float>(rand() % 1020);
            d.speed = 480.f + (rand() % 160);
            d.length = 10.f + (rand() % 8);
        }
    }
}

void WeatherSystem::draw(sf::RenderWindow& window, bool isRaining, bool isInside, float virtualWidth, float virtualHeight)
{
    if (!isRaining || isInside) return;

    // Легкая серовато-голубая пелена дождя
    sf::RectangleShape rainMist({ virtualWidth, virtualHeight });
    rainMist.setFillColor(sf::Color(15, 30, 50, 45));
    window.draw(rainMist);

    // Капли дождя
    for (const auto& d : rainDrops)
    {
        sf::Vertex line[2] = {
            sf::Vertex(d.pos, sf::Color(160, 200, 255, 180)),
            sf::Vertex({ d.pos.x - d.length * 0.25f, d.pos.y + d.length }, sf::Color(200, 230, 255, 230))
        };
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
}
