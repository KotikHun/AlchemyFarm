#pragma once

#include <SFML/Graphics.hpp>
#include "Tool.hpp"

// ============================================
// ИНТЕРФЕЙС (UI)
// ============================================
// Шаг 1: только хотбар. Без текста.
// Постепенно добавим текст и полоску дня.

struct UI {
    // Хотбар
    sf::RectangleShape hotbarBg;
    sf::RectangleShape slotShape;

    // Константы
    static constexpr int HOTBAR_SLOTS = 4;
    static constexpr float SLOT_SIZE = 50.f;
    static constexpr float SLOT_PADDING = 5.f;
    static constexpr float HOTBAR_MARGIN = 10.f;

    // Конструктор
    UI();

    // Отрисовка хотбара
    void drawHotbar(sf::RenderWindow& window, Tool currentTool);
};