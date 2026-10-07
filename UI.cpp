#include "UI.hpp"

// ============================================
// КОНСТРУКТОР
// ============================================
UI::UI()
{
    float hotbarY = 600.f - SLOT_SIZE - HOTBAR_MARGIN;
    float hotbarWidth = HOTBAR_SLOTS * SLOT_SIZE + (HOTBAR_SLOTS + 1) * SLOT_PADDING;
    float hotbarHeight = SLOT_SIZE + 2 * SLOT_PADDING;

    hotbarBg.setSize({ hotbarWidth, hotbarHeight });
    hotbarBg.setFillColor(sf::Color(40, 40, 40));
    hotbarBg.setPosition({ HOTBAR_MARGIN, hotbarY - SLOT_PADDING });

    slotShape.setSize({ SLOT_SIZE, SLOT_SIZE });
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
