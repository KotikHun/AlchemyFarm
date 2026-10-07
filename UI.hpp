#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "Tool.hpp"

// ============================================
// ИНТЕРФЕЙС (UI)
// ============================================
// Хотбар + текст.
// Шрифт загружается в конструкторе ДО создания текстов
// (иначе sf::Text в SFML 3.0 теряет ссылку на шрифт).

struct UI {
    // Шрифт
    sf::Font font;

    // Тексты (создаются в конструкторе ПОСЛЕ загрузки шрифта)
    std::unique_ptr<sf::Text> dayText;
    std::unique_ptr<sf::Text> seedsText;
    std::unique_ptr<sf::Text> leavesText;

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

    // Отрисовка текста (день, семечки, листья)
    void drawTexts(sf::RenderWindow& window,
        int currentDay,
        int seeds,
        int leaves);
};