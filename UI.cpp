#include "UI.hpp"
#include <iostream>

// ============================================
// КОНСТРУКТОР
// ============================================
UI::UI()
{
    // 1. СНАЧАЛА загружаем шрифт
    if (!font.openFromFile("assets/fonts/Roboto-Regular.ttf"))
    {
        sf::err() << "FONT NOT LOADED" << std::endl;
        return;
    }

    // 2. ПОТОМ создаём тексты с уже загруженным шрифтом
    dayText = std::make_unique<sf::Text>(font);
    dayText->setCharacterSize(20);
    dayText->setFillColor(sf::Color::White);
    dayText->setPosition({ 10.f, 20.f });

    seedsText = std::make_unique<sf::Text>(font);
    seedsText->setCharacterSize(20);
    seedsText->setFillColor(sf::Color::White);
    seedsText->setPosition({ 10.f, 45.f });

    leavesText = std::make_unique<sf::Text>(font);
    leavesText->setCharacterSize(20);
    leavesText->setFillColor(sf::Color::White);
    leavesText->setPosition({ 10.f, 70.f });

    // 3. Хотбар
    float hotbarY = 600.f - SLOT_SIZE - HOTBAR_MARGIN;
    float hotbarWidth = HOTBAR_SLOTS * SLOT_SIZE + (HOTBAR_SLOTS + 1) * SLOT_PADDING;
    float hotbarHeight = SLOT_SIZE + 2 * SLOT_PADDING;

    hotbarBg.setSize({ hotbarWidth, hotbarHeight });
    hotbarBg.setFillColor(sf::Color(40, 40, 40));
    hotbarBg.setPosition({ HOTBAR_MARGIN, hotbarY - SLOT_PADDING });

    slotShape.setSize({ SLOT_SIZE, SLOT_SIZE });

    // Полоска дня
    dayBarBg.setSize({ 800.f, 8.f });
    dayBarBg.setFillColor(sf::Color(40, 40, 40));
    dayBarBg.setPosition({ 0.f, 0.f });

    dayBar.setSize({ 800.f, 8.f });
    dayBar.setFillColor(sf::Color(220, 180, 60));
    dayBar.setPosition({ 0.f, 0.f });
}

// ============================================
// ОТРИСОВКА ХОТБАРА
// ============================================
void UI::drawHotbar(sf::RenderWindow& window, Tool currentTool)
{
    window.draw(hotbarBg);

    float hotbarY = 600.f - SLOT_SIZE - HOTBAR_MARGIN;

    for (int i = 0; i < HOTBAR_SLOTS; i++)
    {
        float slotX = HOTBAR_MARGIN + SLOT_PADDING + i * (SLOT_SIZE + SLOT_PADDING);
        slotShape.setPosition({ slotX, hotbarY });

        bool isSelected = false;
        if (i == 0 && currentTool == Tool::Hoe) isSelected = true;
        if (i == 1 && currentTool == Tool::Seed) isSelected = true;
        if (i == 2 && currentTool == Tool::WateringCan) isSelected = true;
        if (i == 3 && currentTool == Tool::Hand) isSelected = true;

        if (isSelected)
            slotShape.setFillColor(sf::Color(200, 200, 100));
        else
            slotShape.setFillColor(sf::Color(80, 80, 80));

        window.draw(slotShape);
    }
}

// ============================================
// ОТРИСОВКА ТЕКСТА
// ============================================
void UI::drawTexts(sf::RenderWindow& window,
    int currentDay,
    int seeds,
    int leaves)
{
    if (dayText)
    {
        dayText->setString("Day: " + std::to_string(currentDay));
        window.draw(*dayText);
    }

    if (seedsText)
    {
        seedsText->setString("Seeds: " + std::to_string(seeds));
        window.draw(*seedsText);
    }

    if (leavesText)
    {
        leavesText->setString("Leaves: " + std::to_string(leaves));
        window.draw(*leavesText);
    }
}


// ============================================
// ОТРИСОВКА ПОЛОСКИ ДНЯ
// ============================================
void UI::drawDayBar(sf::RenderWindow& window, float dayProgress)
{
    window.draw(dayBarBg);
    dayBar.setSize({ 800.f * dayProgress, 8.f });
    window.draw(dayBar);
}