#pragma once

#include <SFML/Graphics.hpp>
#include "World.hpp"
#include "Player.hpp"
#include "Inventory.hpp"
#include "UI.hpp"
#include "Tool.hpp"

// ============================================
// ИГРА
// ============================================
// Хранит окно, все системы и игровой цикл.

struct Game {
    // Окно
    sf::RenderWindow window;

    // Системы
    World world;
    Player player;
    Inventory inv;
    UI ui;

    // Текущий инструмент
    Tool currentTool = Tool::Hoe;

    // Маркер тайла перед игроком
    sf::RectangleShape targetMarker;

    // День и время
    static constexpr float DAY_LENGTH = 60.f;
    float dayTimer = 0.f;
    int currentDay = 1;

    // Часы
    sf::Clock clock;

    // Конструктор — создаёт окно, маркер
    Game();

    // Игровой цикл
    void run();

private:
    // Обработка событий
    void handleEvents();

    // Обновление (день, игрок)
    void update(float dt);

    // Отрисовка (мир, игрок, UI)
    void render();
};