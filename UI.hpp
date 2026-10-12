#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "Inventory.hpp"

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

    // Полоска дня
    sf::RectangleShape dayBarBg;
    sf::RectangleShape dayBar;

    // Окно инвентаря
    sf::RectangleShape inventoryBg;

    // Корзина для удаления
    sf::RectangleShape trashSlot;

    // Константы
    static constexpr int HOTBAR_SLOTS = 9;
    static constexpr float SLOT_SIZE = 50.f;
    static constexpr float SLOT_PADDING = 5.f;
    static constexpr float HOTBAR_MARGIN = 10.f;

    // Конструктор
    UI();

    // Получить цвет для типа предмета
    static sf::Color getItemColor(const std::string& id);

    // Получить букву для типа предмета
    static std::string getItemLetter(const std::string& id);

    // Нарисовать иконку предмета в слоте
    void drawItemIcon(sf::RenderWindow& window,
        float x, float y,
        const Item& item);

    // Отрисовка хотбара    
    void drawHotbar(sf::RenderWindow& window, const Inventory& inv, int activeSlot);

    // Отрисовка текста (день, семечки, листья)
    void drawTexts(sf::RenderWindow& window,
        int currentDay,
        int seeds,
        int leaves);
    // Отрисовка полоски дня
 // dayProgress — от 0.0 до 1.0
    void drawDayBar(sf::RenderWindow& window, float dayProgress);

    // Отрисовка окна инвентаря (рюкзак + хотбар)
    // isOpen — рисовать ли (иначе ничего)
    void drawInventory(sf::RenderWindow& window,
        const Inventory& inv,
        int activeSlot,
        bool isOpen,
        bool isDragging,
        bool chestOpen = false);

    // Отрисовка открытого сундука (сетка 9×2 сверху)
    void drawChest(sf::RenderWindow& window,
        const Inventory& chestInv,
        int slotCount,
        bool isOpen);
};