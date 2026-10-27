#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// ============================================
// ВСПЛЫВАЮЩИЙ ТЕКСТ (FLOATING TEXT)
// ============================================
// Появляется при подборе предмета (+2 Камень, +1 Ягода),
// плавно поднимается вверх и растворяется.

struct FloatingText {
    std::string text;
    sf::Vector2f position;
    float timer = 1.6f;
    float maxTime = 1.6f;
    sf::Color color = sf::Color(255, 255, 220);
};
