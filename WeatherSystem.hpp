#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// ============================================
// СИСТЕМА ПОГОДЫ И ДОЖДЯ
// ============================================

struct RainDrop {
    sf::Vector2f pos;
    float speed = 520.f;
    float length = 14.f;
};

class WeatherSystem {
public:
    std::vector<RainDrop> rainDrops;

    WeatherSystem();

    void init();
    void update(float dt, bool isRaining);
    void draw(sf::RenderWindow& window, bool isRaining, bool isInside, float virtualWidth, float virtualHeight);
};
