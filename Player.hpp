#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// ============================================
// ИГРОК
// ============================================
// Хранит состояние игрока и умеет обновляться
// (двигаться) и рисоваться.

struct Player {
    // Размер и скорость
    static constexpr float SIZE = 24.f;
    static constexpr float SPEED = 150.f;

    // Внешний вид
    sf::RectangleShape shape;

    // Направление взгляда (для взаимодействия)
    int dirX = 0;
    int dirY = -1;  // по умолчанию вверх

    // Конструктор — создаёт shape и задаёт параметры
    Player();

    // Обновление (движение) за кадр
    // windowWidth, windowHeight — для ограничения по краям
    void update(float dt, float windowWidth, float windowHeight,
        const std::vector<sf::FloatRect>& obstacles);

    // Отрисовка
    void draw(sf::RenderWindow& window);
};