#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include "Tile.hpp"

// ============================================
// ИГРОВОЙ МИР (КАРТА)
// ============================================
// Хранит карту из тайлов и умеет её рисовать.
// Позже сюда добавятся рост растений
// и взаимодействие с тайлами.

struct World {
    // Размер карты (в тайлах)
    static constexpr int WIDTH = 30;
    static constexpr int HEIGHT = 25;
    static constexpr float TILE_SIZE = 32.f;

    // Карта — двумерный массив тайлов
    std::array<std::array<Tile, WIDTH>, HEIGHT> map;

    // Служебный прямоугольник для отрисовки тайлов
    sf::RectangleShape tileShape;

    // Цвета тайлов
    static constexpr int GRASS_R = 60;
    static constexpr int GRASS_G = 130;
    static constexpr int GRASS_B = 60;

    static constexpr int SOIL_R = 139;
    static constexpr int SOIL_G = 90;
    static constexpr int SOIL_B = 43;

    static constexpr int WATERED_SOIL_R = 100;
    static constexpr int WATERED_SOIL_G = 65;
    static constexpr int WATERED_SOIL_B = 30;

    // Конструктор — создаёт карту и заполняет травой
    World();

    // Отрисовка карты
    void draw(sf::RenderWindow& window);
};