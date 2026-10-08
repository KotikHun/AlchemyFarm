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

    // Фон окна инвентаря (полупрозрачный тёмный)
    inventoryBg.setSize({ 500.f, 240.f });  // 27 слотов (9x3) + хотбар
    inventoryBg.setFillColor(sf::Color(30, 30, 30, 220));
    inventoryBg.setPosition({ 150.f, 180.f });

    slotShape.setSize({ SLOT_SIZE, SLOT_SIZE });
}

// ============================================
// ОТРИСОВКА ХОТБАРА
// ============================================
void UI::drawHotbar(sf::RenderWindow& window, const Inventory& inv, int activeSlot)
{
    window.draw(hotbarBg);

    float hotbarY = 600.f - SLOT_SIZE - HOTBAR_MARGIN;

    for (int i = 0; i < HOTBAR_SLOTS; i++)
    {
        float slotX = HOTBAR_MARGIN + SLOT_PADDING + i * (SLOT_SIZE + SLOT_PADDING);
        slotShape.setPosition({ slotX, hotbarY });

        // Подсветка по индексу активного слота
        if (i == activeSlot)
            slotShape.setFillColor(sf::Color(200, 200, 100));
        else
            slotShape.setFillColor(sf::Color(80, 80, 80));

        window.draw(slotShape);

        // Иконка предмета (если не пусто)
        const Item& item = inv.getSlot(i);
        drawItemIcon(window, slotX, hotbarY, item);
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


// ============================================
// ОТРИСОВКА ИНВЕНТАРЯ
// ============================================
void UI::drawInventory(sf::RenderWindow& window,
    const Inventory& inv,
    int activeSlot,
    bool isOpen)
{
    if (!isOpen) return;

    // Фон
    window.draw(inventoryBg);

    // Позиция сетки: рюкзак (9x3) сверху, хотбар (9x1) снизу
    const float GRID_X = 150.f + SLOT_PADDING;
    const float GRID_Y = 180.f + SLOT_PADDING;

    // Рюкзак: слоты 9-35 (27 штук)
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float x = GRID_X + col * (SLOT_SIZE + SLOT_PADDING);
            float y = GRID_Y + row * (SLOT_SIZE + SLOT_PADDING);

            slotShape.setPosition({ x, y });
            slotShape.setFillColor(sf::Color(60, 60, 60));
            window.draw(slotShape);

            // Иконка предмета
            int slotIndex = 9 + row * 9 + col;
            const Item& item = inv.getSlot(slotIndex);
            drawItemIcon(window, x, y, item);
        }
    }

    // Хотбар: слоты 0-8 (9 штук)
    float hotbarY = GRID_Y + 3 * (SLOT_SIZE + SLOT_PADDING) + 5.f;
    for (int col = 0; col < 9; col++)
    {
        float x = GRID_X + col * (SLOT_SIZE + SLOT_PADDING);
        float y = hotbarY;

        slotShape.setPosition({ x, y });

        // Подсветка активного слота
        if (col == activeSlot)
            slotShape.setFillColor(sf::Color(200, 200, 100));
        else
            slotShape.setFillColor(sf::Color(60, 60, 60));

        window.draw(slotShape);

        // Иконка предмета
        const Item& item = inv.getSlot(col);
        drawItemIcon(window, x, y, item);
    }
}

// ============================================
// ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ
// ============================================
sf::Color UI::getItemColor(const std::string& id)
{
    if (id == "hoe") return sf::Color(139, 90, 43);
    if (id == "watering_can") return sf::Color(70, 130, 220);
    if (id == "seeds") return sf::Color(100, 180, 100);
    if (id == "leaves") return sf::Color(150, 220, 100);
    return sf::Color(120, 120, 120);
}

std::string UI::getItemLetter(const std::string& id)
{
    if (id == "hoe") return "H";
    if (id == "watering_can") return "W";
    if (id == "seeds") return "S";
    if (id == "leaves") return "L";
    return "?";
}

void UI::drawItemIcon(sf::RenderWindow& window,
    float x, float y,
    const Item& item)
{
    if (item.isEmpty()) return;

    // Цветной квадрат
    sf::RectangleShape icon({ SLOT_SIZE - 10.f, SLOT_SIZE - 10.f });
    icon.setPosition({ x + 5.f, y + 5.f });
    icon.setFillColor(getItemColor(item.id));
    window.draw(icon);

    // Буква
    sf::Text letter(font);
    letter.setString(getItemLetter(item.id));
    letter.setCharacterSize(22);
    letter.setFillColor(sf::Color::White);
    letter.setPosition({ x + 16.f, y + 10.f });
    window.draw(letter);

    // Количество (если > 1)
    if (item.count > 1)
    {
        sf::Text countText(font);
        countText.setString(std::to_string(item.count));
        countText.setCharacterSize(14);
        countText.setFillColor(sf::Color::White);
        countText.setPosition({ x + 32.f, y + 32.f });
        window.draw(countText);
    }
}