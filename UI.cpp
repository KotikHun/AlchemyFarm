#include "UI.hpp"
#include "Player.hpp"
#include "AlchemySystem.hpp"
#include <iostream>
#include <cmath>

static bool isInside(float mx, float my, float x, float y, float size)
{
    return mx >= x && mx < x + size && my >= y && my < y + size;
}

static bool isInsideRect(float mx, float my, float x, float y, float w, float h)
{
    return mx >= x && mx < x + w && my >= y && my < y + h;
}

// ============================================
// КОНСТРУКТОР
// ============================================
UI::UI()
{
    // 1. Шрифт
    if (!font.openFromFile("assets/fonts/Roboto-Regular.ttf"))
    {
        sf::err() << "FONT NOT LOADED" << std::endl;
        return;
    }

    // 2. Иконки предметов
    loadIcon("hoe", "assets/sprites/icons/hoe.png");
    loadIcon("watering_can", "assets/sprites/icons/watering_can.png");
    loadIcon("axe", "assets/sprites/icons/axe.png");
    loadIcon("pickaxe", "assets/sprites/icons/pickaxe.png");
    loadIcon("seeds", "assets/sprites/icons/seeds.png");
    loadIcon("leaves", "assets/sprites/icons/leaves.png");
    loadIcon("berry", "assets/sprites/icons/berry.png");
    loadIcon("wood", "assets/sprites/icons/wood.png");
    loadIcon("stone", "assets/sprites/icons/stone.png");
    loadIcon("fiber", "assets/sprites/icons/fiber.png");
    loadIcon("trash", "assets/sprites/icons/trash.png");
    loadIcon("energy_potion", "assets/sprites/icons/energy_potion.png");
    loadIcon("speed_potion", "assets/sprites/icons/speed_potion.png");
    loadIcon("growth_potion", "assets/sprites/icons/growth_potion.png");
    loadIcon("empty_flask", "assets/sprites/icons/empty_flask.png");
    loadIcon("water_flask", "assets/sprites/icons/water_flask.png");
    loadIcon("berry_powder", "assets/sprites/icons/berry_powder.png");
    loadIcon("herbal_powder", "assets/sprites/icons/herbal_powder.png");
    loadIcon("alchemist_table", "assets/sprites/icons/alchemist_table.png");
    loadIcon("chest", "assets/sprites/icons/chest.png");
    loadIcon("bed", "assets/sprites/icons/bed.png");
    loadIcon("cauldron", "assets/sprites/icons/cauldron.png");
    loadIcon("carpet", "assets/sprites/icons/carpet.png");
    loadIcon("window", "assets/sprites/icons/window.png");

    // 3. Тексты статистики
    dayText = std::make_unique<sf::Text>(font);
    dayText->setCharacterSize(18);
    dayText->setFillColor(sf::Color::White);
    dayText->setPosition({ 15.f, 15.f });

    seedsText = std::make_unique<sf::Text>(font);
    seedsText->setCharacterSize(18);
    seedsText->setFillColor(sf::Color(160, 220, 160));
    seedsText->setPosition({ 15.f, 38.f });

    leavesText = std::make_unique<sf::Text>(font);
    leavesText->setCharacterSize(18);
    leavesText->setFillColor(sf::Color(255, 160, 160));
    leavesText->setPosition({ 15.f, 61.f });

    // 4. Панели
    float hotbarY = 600.f - SLOT_SIZE - HOTBAR_MARGIN;
    float hotbarWidth = HOTBAR_SLOTS * SLOT_SIZE + (HOTBAR_SLOTS + 1) * SLOT_PADDING;
    float hotbarHeight = SLOT_SIZE + 2 * SLOT_PADDING;

    hotbarBg.setSize({ hotbarWidth, hotbarHeight });
    hotbarBg.setFillColor(sf::Color(25, 25, 30, 240));
    hotbarBg.setOutlineColor(sf::Color(140, 105, 65));
    hotbarBg.setOutlineThickness(2.f);
    hotbarBg.setPosition({ HOTBAR_MARGIN, hotbarY - SLOT_PADDING });

    slotShape.setSize({ SLOT_SIZE, SLOT_SIZE });

    // Полоска дня
    dayBarBg.setSize({ 800.f, 8.f });
    dayBarBg.setFillColor(sf::Color(30, 30, 35));
    dayBarBg.setPosition({ 0.f, 0.f });

    dayBar.setSize({ 800.f, 8.f });
    dayBar.setFillColor(sf::Color(235, 185, 55));
    dayBar.setPosition({ 0.f, 0.f });

    // Фон окна инвентаря — аккуратно вмещает рюкзак, хотбар и корзину
    inventoryBg.setSize({ 510.f, 290.f });
    inventoryBg.setFillColor(sf::Color(25, 25, 32, 248));
    inventoryBg.setOutlineColor(sf::Color(150, 115, 75));
    inventoryBg.setOutlineThickness(2.f);
    inventoryBg.setPosition({ 145.f, 175.f });

    // Корзина
    trashSlot.setSize({ SLOT_SIZE, SLOT_SIZE });
}

// ============================================
// ЗАГРУЗКА ИКОНКИ
// ============================================
void UI::loadIcon(const std::string& id, const std::string& path)
{
    sf::Texture tex;
    if (tex.loadFromFile(path))
    {
        itemIcons[id] = std::move(tex);
    }
}

// ============================================
// ОТРИСОВКА ХОТБАРА С ПОДСВЕТКОЙ
// ============================================
void UI::drawHotbar(sf::RenderWindow& window, const Inventory& inv, int activeSlot, const ItemDatabase& itemDB)
{
    float totalHotbarWidth = HOTBAR_SLOTS * SLOT_SIZE + (HOTBAR_SLOTS + 1) * SLOT_PADDING;
    float hotbarHeight = SLOT_SIZE + 2 * SLOT_PADDING;
    float hotbarX = (static_cast<float>(window.getSize().x) - totalHotbarWidth) / 2.f;
    float hotbarY = static_cast<float>(window.getSize().y) - SLOT_SIZE - 20.f;

    hotbarBg.setSize({ totalHotbarWidth, hotbarHeight });
    hotbarBg.setPosition({ hotbarX, hotbarY - SLOT_PADDING });
    window.draw(hotbarBg);

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mousePixel.x);
    float my = static_cast<float>(mousePixel.y);

    for (int i = 0; i < HOTBAR_SLOTS; i++)
    {
        float slotX = hotbarX + SLOT_PADDING + i * (SLOT_SIZE + SLOT_PADDING);
        slotShape.setPosition({ slotX, hotbarY });

        bool hovered = isInside(mx, my, slotX, hotbarY, SLOT_SIZE);
        const Item& item = inv.getSlot(i);

        if (hovered && !item.isEmpty())
        {
            const ItemData* d = itemDB.find(item.id);
            if (d) hoveredItemName = d->name;
            else hoveredItemName = item.id;
        }

        if (i == activeSlot)
        {
            slotShape.setFillColor(sf::Color(55, 50, 42));
            slotShape.setOutlineColor(sf::Color(255, 215, 0));
            slotShape.setOutlineThickness(2.f);
        }
        else if (hovered)
        {
            slotShape.setFillColor(sf::Color(58, 62, 75));
            slotShape.setOutlineColor(sf::Color(140, 200, 255));
            slotShape.setOutlineThickness(2.f);
        }
        else
        {
            slotShape.setFillColor(sf::Color(42, 42, 50));
            slotShape.setOutlineColor(sf::Color(70, 70, 80));
            slotShape.setOutlineThickness(1.f);
        }

        window.draw(slotShape);
        drawItemIcon(window, slotX, hotbarY, item);
    }
}

// ============================================
// КРАСИВЫЙ HUD (ВРЕМЯ, ДЕНЬ, ЗОЛОТО)
// ============================================
void UI::drawHUD(sf::RenderWindow& window, int currentDay, const std::string& timeStr, bool isCurfew, int hour)
{
    float hudW = 142.f;
    float hudH = 68.f;
    float hudX = static_cast<float>(window.getSize().x) - hudW - 20.f;
    float hudY = 16.f;

    // Панель часов
    sf::RectangleShape hudBg({ hudW, hudH });
    hudBg.setPosition({ hudX, hudY });
    hudBg.setFillColor(sf::Color(22, 24, 30, 230));
    hudBg.setOutlineColor(isCurfew ? sf::Color(255, 80, 80) : sf::Color(180, 140, 70));
    hudBg.setOutlineThickness(2.f);
    window.draw(hudBg);

    // День
    sf::Text dayTxt(font);
    dayTxt.setString(toUtf8("День " + std::to_string(currentDay)));
    dayTxt.setCharacterSize(16);
    dayTxt.setFillColor(sf::Color(255, 220, 120));
    dayTxt.setPosition({ hudX + 12.f, hudY + 8.f });
    window.draw(dayTxt);

    // Время (06:00 ... 02:00)
    sf::Text timeTxt(font);
    timeTxt.setString(timeStr);
    timeTxt.setCharacterSize(16);
    // При комендантском часе (после 00:00) время горит красным, предупреждая игрока!
    timeTxt.setFillColor(isCurfew ? sf::Color(255, 65, 65) : sf::Color(240, 240, 245));
    timeTxt.setPosition({ hudX + 12.f, hudY + 34.f });
    window.draw(timeTxt);

    // Значок времени дня (солнце / луна)
    sf::CircleShape sun(7.f);
    sun.setPosition({ hudX + 70.f, hudY + 38.f });
    if (hour >= 6 && hour < 19) {
        sun.setFillColor(sf::Color(255, 215, 60)); // Солнце
    } else {
        sun.setFillColor(sf::Color(160, 200, 255)); // Луна
    }
    window.draw(sun);

    // Монеты / золото
    sf::Text goldTxt(font);
    goldTxt.setString(toUtf8("0 G"));
    goldTxt.setCharacterSize(14);
    goldTxt.setFillColor(sf::Color(255, 215, 90));
    goldTxt.setPosition({ hudX + 98.f, hudY + 36.f });
    window.draw(goldTxt);
}

// ============================================
// ОТРИСОВКА ТЕКСТА (УСТАРЕВШЕЕ, ДЛЯ СОВМЕСТИМОСТИ)
// ============================================
void UI::drawTexts(sf::RenderWindow& window,
    int currentDay,
    int seeds,
    int leaves)
{
    // Сырые счетчики семян и ягод убраны по просьбе игрока ("лишнее убери")
}

// ============================================
// ПОЛОСКА ДНЯ
// ============================================
void UI::drawDayBar(sf::RenderWindow& window, float dayProgress)
{
    // Заменено на красивый HUD с часами
}

// ============================================
// ИНВЕНТАРЬ (РЮКЗАК + ВКЛАДКИ)
// ============================================
void UI::drawInventory(sf::RenderWindow& window,
    const Inventory& inv,
    int activeSlot,
    bool isOpen,
    bool isDragging,
    bool chestOpen,
    const ItemDatabase& itemDB,
    int currentTab,
    int journalCategory,
    const AlchemySystem* alchemy)
{
    if (!isOpen) return;

    if (chestOpen)
    {
        inventoryBg.setSize({ 510.f, 290.f });
        inventoryBg.setPosition({ 225.f, 305.f });
        window.draw(inventoryBg);

        sf::Text invTitle(font);
        invTitle.setString(toUtf8("Инвентарь"));
        invTitle.setCharacterSize(15);
        invTitle.setFillColor(sf::Color(220, 220, 240));
        invTitle.setPosition({ 235.f, 313.f });
        window.draw(invTitle);
    }
    else
    {
        // Вкладки сверху инвентаря
        const char* tabNames[] = { "Рюкзак", "Крафт", "Дневник" };
        sf::Vector2i mpos = sf::Mouse::getPosition(window);
        float mx = static_cast<float>(mpos.x);
        float my = static_cast<float>(mpos.y);

        for (int t = 0; t < 3; t++)
        {
            float tabX = 225.f + t * 105.f;
            float tabY = 175.f;
            float tabW = 100.f;
            float tabH = 32.f;

            bool tabHovered = isInsideRect(mx, my, tabX, tabY, tabW, tabH);

            sf::RectangleShape tabShape({ tabW, tabH });
            tabShape.setPosition({ tabX, tabY });
            if (t == currentTab)
            {
                tabShape.setFillColor(sf::Color(45, 42, 50));
                tabShape.setOutlineColor(sf::Color(255, 215, 0));
                tabShape.setOutlineThickness(2.f);
            }
            else
            {
                tabShape.setFillColor(tabHovered ? sf::Color(38, 38, 44) : sf::Color(26, 26, 30));
                tabShape.setOutlineColor(sf::Color(90, 80, 70));
                tabShape.setOutlineThickness(1.5f);
            }
            window.draw(tabShape);

            sf::Text tTxt(font);
            tTxt.setString(toUtf8(tabNames[t]));
            tTxt.setCharacterSize(14);
            tTxt.setFillColor((t == currentTab) ? sf::Color(255, 230, 130) : sf::Color(170, 170, 170));
            sf::FloatRect tb = tTxt.getLocalBounds();
            tTxt.setPosition({ tabX + (tabW - tb.size.x) / 2.f, tabY + 7.f });
            window.draw(tTxt);
        }

        inventoryBg.setSize({ 510.f, 290.f });
        inventoryBg.setPosition({ 225.f, 210.f });
        window.draw(inventoryBg);
    }

    if (!chestOpen && currentTab == 1)
    {
        drawCraftingTab(window, inv, itemDB);
        return;
    }
    if (!chestOpen && currentTab == 2)
    {
        drawJournalTab(window, journalCategory, alchemy);
        return;
    }

    const float GRID_X = 235.f;
    float GRID_Y = chestOpen ? 340.f : 220.f;

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mousePixel.x);
    float my = static_cast<float>(mousePixel.y);

    // 1. Рюкзак: слоты 9-35
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float x = GRID_X + col * (SLOT_SIZE + SLOT_PADDING);
            float y = GRID_Y + row * (SLOT_SIZE + SLOT_PADDING);

            slotShape.setPosition({ x, y });

            bool hovered = isInside(mx, my, x, y, SLOT_SIZE);
            int slotIndex = 9 + row * 9 + col;
            const Item& item = inv.getSlot(slotIndex);

            if (hovered && !item.isEmpty() && !isDragging)
            {
                const ItemData* d = itemDB.find(item.id);
                if (d) hoveredItemName = d->name;
            }

            if (hovered)
            {
                slotShape.setFillColor(sf::Color(60, 65, 80));
                slotShape.setOutlineColor(sf::Color(140, 200, 255));
                slotShape.setOutlineThickness(2.f);
            }
            else
            {
                slotShape.setFillColor(sf::Color(45, 45, 52));
                slotShape.setOutlineColor(sf::Color(70, 70, 80));
                slotShape.setOutlineThickness(1.f);
            }
            window.draw(slotShape);
            drawItemIcon(window, x, y, item);
        }
    }

    // 2. Хотбар в инвентаре: слоты 0-8
    float hotbarY = GRID_Y + 3 * (SLOT_SIZE + SLOT_PADDING) + 5.f;
    for (int col = 0; col < 9; col++)
    {
        float x = GRID_X + col * (SLOT_SIZE + SLOT_PADDING);
        float y = hotbarY;

        slotShape.setPosition({ x, y });

        bool hovered = isInside(mx, my, x, y, SLOT_SIZE);
        const Item& item = inv.getSlot(col);

        if (hovered && !item.isEmpty() && !isDragging)
        {
            const ItemData* d = itemDB.find(item.id);
            if (d) hoveredItemName = d->name;
        }

        if (col == activeSlot)
        {
            slotShape.setFillColor(sf::Color(55, 50, 42));
            slotShape.setOutlineColor(sf::Color(255, 215, 0));
            slotShape.setOutlineThickness(2.f);
        }
        else if (hovered)
        {
            slotShape.setFillColor(sf::Color(60, 65, 80));
            slotShape.setOutlineColor(sf::Color(140, 200, 255));
            slotShape.setOutlineThickness(2.f);
        }
        else
        {
            slotShape.setFillColor(sf::Color(45, 45, 52));
            slotShape.setOutlineColor(sf::Color(70, 70, 80));
            slotShape.setOutlineThickness(1.f);
        }
        window.draw(slotShape);
        drawItemIcon(window, x, y, item);
    }

    // 3. Корзина (аккуратно внутри нижней панели инвентаря)
    if (!chestOpen)
    {
        float trashX = 665.f;
        float trashY = hotbarY + SLOT_SIZE + 10.f;

        trashSlot.setPosition({ trashX, trashY });

        bool hoveredTrash = isInside(mx, my, trashX, trashY, SLOT_SIZE);
        if (hoveredTrash)
        {
            hoveredItemName = "Корзина (удалить)";
        }

        if (hoveredTrash && isDragging)
        {
            trashSlot.setFillColor(sf::Color(130, 30, 35));
            trashSlot.setOutlineColor(sf::Color(255, 80, 80));
            trashSlot.setOutlineThickness(2.5f);
        }
        else if (hoveredTrash)
        {
            trashSlot.setFillColor(sf::Color(70, 35, 40));
            trashSlot.setOutlineColor(sf::Color(220, 80, 80));
            trashSlot.setOutlineThickness(2.f);
        }
        else
        {
            trashSlot.setFillColor(sf::Color(45, 30, 35, 230));
            trashSlot.setOutlineColor(sf::Color(140, 60, 60));
            trashSlot.setOutlineThickness(1.5f);
        }
        window.draw(trashSlot);

        // Иконка корзины внутри слота
        auto itTrash = itemIcons.find("trash");
        if (itTrash != itemIcons.end())
        {
            sf::Sprite trashSpr(itTrash->second);
            trashSpr.setPosition({ trashX + 9.f, trashY + 9.f });
            window.draw(trashSpr);
        }

        // Поясняющая надпись рядом с корзиной
        sf::Text trashLabel(font);
        trashLabel.setString(toUtf8("Корзина:"));
        trashLabel.setCharacterSize(14);
        trashLabel.setFillColor(sf::Color(180, 140, 140));
        trashLabel.setPosition({ trashX - 68.f, trashY + 16.f });
        window.draw(trashLabel);
    }
}

// ============================================
// СУНДУК
// ============================================
void UI::drawChest(sf::RenderWindow& window,
    const Inventory& chestInv,
    int slotCount,
    bool isOpen,
    const ItemDatabase& itemDB)
{
    if (!isOpen) return;

    sf::RectangleShape chestBg({ 510.f, 155.f });
    chestBg.setFillColor(sf::Color(30, 25, 22, 245));
    chestBg.setOutlineColor(sf::Color(160, 110, 60));
    chestBg.setOutlineThickness(2.f);
    chestBg.setPosition({ 225.f, 130.f });
    window.draw(chestBg);

    sf::Text title(font);
    title.setString(toUtf8("Сундук"));
    title.setCharacterSize(15);
    title.setFillColor(sf::Color(255, 220, 150));
    title.setPosition({ 235.f, 138.f });
    window.draw(title);

    const float GRID_X = 235.f;
    const float GRID_Y = 165.f;

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mousePixel.x);
    float my = static_cast<float>(mousePixel.y);

    int rows = slotCount / 9;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float x = GRID_X + col * (SLOT_SIZE + SLOT_PADDING);
            float y = GRID_Y + row * (SLOT_SIZE + SLOT_PADDING);

            slotShape.setPosition({ x, y });

            bool hovered = isInside(mx, my, x, y, SLOT_SIZE);
            int slotIndex = row * 9 + col;
            const Item& item = chestInv.getSlot(slotIndex);

            if (hovered && !item.isEmpty())
            {
                const ItemData* d = itemDB.find(item.id);
                if (d) hoveredItemName = d->name;
            }

            if (hovered)
            {
                slotShape.setFillColor(sf::Color(65, 60, 75));
                slotShape.setOutlineColor(sf::Color(140, 200, 255));
                slotShape.setOutlineThickness(2.f);
            }
            else
            {
                slotShape.setFillColor(sf::Color(45, 42, 45));
                slotShape.setOutlineColor(sf::Color(80, 70, 70));
                slotShape.setOutlineThickness(1.f);
            }
            window.draw(slotShape);
            drawItemIcon(window, x, y, item);
        }
    }
}

// ============================================
// ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ (ФОЛЛБЭК ЦВЕТОВ)
// ============================================
sf::Color UI::getItemColor(const std::string& id)
{
    if (id == "hoe") return sf::Color(139, 90, 43);
    if (id == "watering_can") return sf::Color(70, 130, 220);
    if (id == "axe") return sf::Color(180, 70, 40);
    if (id == "pickaxe") return sf::Color(100, 100, 130);
    if (id == "seeds") return sf::Color(100, 180, 100);
    if (id == "leaves") return sf::Color(150, 220, 100);
    if (id == "berry") return sf::Color(220, 50, 60);
    if (id == "wood") return sf::Color(150, 100, 50);
    if (id == "stone") return sf::Color(120, 120, 120);
    if (id == "fiber") return sf::Color(80, 160, 60);
    if (id == "chest") return sf::Color(140, 95, 50);
    if (id == "bed") return sf::Color(160, 70, 70);
    if (id == "cauldron") return sf::Color(60, 60, 70);
    if (id == "carpet") return sf::Color(80, 70, 90);
    if (id == "window") return sf::Color(220, 200, 130);
    if (id == "alchemist_table") return sf::Color(150, 110, 60);
    return sf::Color(120, 120, 120);
}

std::string UI::getItemLetter(const std::string& id)
{
    if (id == "hoe") return "H";
    if (id == "watering_can") return "W";
    if (id == "axe") return "A";
    if (id == "pickaxe") return "P";
    if (id == "seeds") return "S";
    if (id == "leaves") return "L";
    if (id == "berry") return "B";
    if (id == "wood") return "Wd";
    if (id == "stone") return "St";
    if (id == "fiber") return "Fb";
    if (id == "chest") return "Ch";
    if (id == "bed") return "Bd";
    if (id == "cauldron") return "Cd";
    if (id == "carpet") return "Cp";
    if (id == "window") return "Wn";
    if (id == "alchemist_table") return "Tb";
    return "?";
}

// ============================================
// ОТРИСОВКА ИКОНКИ
// ============================================
void UI::drawItemIcon(sf::RenderWindow& window,
    float x, float y,
    const Item& item)
{
    if (item.isEmpty()) return;

    auto it = itemIcons.find(item.id);
    if (it != itemIcons.end())
    {
        sf::Sprite sprite(it->second);
        sprite.setPosition({ x + 9.f, y + 9.f });
        window.draw(sprite);
    }
    else
    {
        sf::RectangleShape icon({ SLOT_SIZE - 10.f, SLOT_SIZE - 10.f });
        icon.setPosition({ x + 5.f, y + 5.f });
        icon.setFillColor(getItemColor(item.id));
        window.draw(icon);

        sf::Text letter(font);
        letter.setString(getItemLetter(item.id));
        letter.setCharacterSize(18);
        letter.setFillColor(sf::Color::White);
        letter.setPosition({ x + 12.f, y + 10.f });
        window.draw(letter);
    }

    if (item.count > 1)
    {
        sf::Text shadow(font);
        shadow.setString(std::to_string(item.count));
        shadow.setCharacterSize(14);
        shadow.setFillColor(sf::Color(0, 0, 0, 230));
        shadow.setPosition({ x + 31.f, y + 29.f });
        window.draw(shadow);

        sf::Text countText(font);
        countText.setString(std::to_string(item.count));
        countText.setCharacterSize(14);
        countText.setFillColor(sf::Color::White);
        countText.setPosition({ x + 30.f, y + 28.f });
        window.draw(countText);
    }
}

// ============================================
// ДРОП НА ЗЕМЛЕ
// ============================================
void UI::drawDroppedItem(sf::RenderWindow& window, const DroppedItem& drop)
{
    float bob = std::sin(drop.age * 4.f) * 3.f;
    float x = drop.position.x;
    float y = drop.position.y + bob;

    sf::CircleShape shadow(7.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 80));
    shadow.setScale({ 1.4f, 0.6f });
    shadow.setPosition({ x + 3.f, drop.position.y + 16.f });
    window.draw(shadow);

    auto it = itemIcons.find(drop.item.id);
    if (it != itemIcons.end())
    {
        sf::Sprite sprite(it->second);
        sprite.setScale({ 22.f / 32.f, 22.f / 32.f });
        sprite.setPosition({ x + 4.f, y });
        window.draw(sprite);
    }
    else
    {
        sf::RectangleShape icon({ 22.f, 22.f });
        icon.setPosition({ x + 4.f, y });
        icon.setFillColor(getItemColor(drop.item.id));
        window.draw(icon);

        sf::Text letter(font);
        letter.setString(getItemLetter(drop.item.id));
        letter.setCharacterSize(13);
        letter.setFillColor(sf::Color::White);
        letter.setPosition({ x + 9.f, y + 2.f });
        window.draw(letter);
    }
}

// ============================================
// УВЕДОМЛЕНИЕ ПРИ ВЗЯТИИ В РУКИ (UTF-8 РУССКИЙ)
// ============================================
void UI::drawActiveItemNotification(sf::RenderWindow& window, const std::string& itemName, float timer)
{
    if (itemName.empty() || timer <= 0.f) return;

    float alphaFactor = (timer > 1.2f) ? 1.f : (timer / 1.2f);
    uint8_t a = static_cast<uint8_t>(std::clamp(alphaFactor * 255.f, 0.f, 255.f));

    sf::Text text(font);
    text.setString(toUtf8(itemName));
    text.setCharacterSize(18);

    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({ bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });

    float centerX = static_cast<float>(window.getSize().x) / 2.f;
    float posY = static_cast<float>(window.getSize().y) - SLOT_SIZE - 20.f - 24.f;

    // Черная тень
    text.setFillColor(sf::Color(0, 0, 0, a));
    text.setPosition({ centerX + 1.f, posY + 1.f });
    window.draw(text);

    // Белый основной текст
    text.setFillColor(sf::Color(255, 255, 255, a));
    text.setPosition({ centerX, posY });
    window.draw(text);
}

// ============================================
// ВСПЛЫВАЮЩИЙ ТЕКСТ ПОДБОРА (UTF-8 РУССКИЙ)
// ============================================
void UI::drawFloatingTexts(sf::RenderWindow& window, const std::vector<FloatingText>& texts)
{
    for (const auto& ft : texts)
    {
        float alphaFactor = std::min(1.f, ft.timer / 0.4f);
        uint8_t a = static_cast<uint8_t>(alphaFactor * 255.f);

        sf::Text shadow(font);
        shadow.setString(toUtf8(ft.text));
        shadow.setCharacterSize(15);
        shadow.setFillColor(sf::Color(0, 0, 0, a));
        shadow.setPosition({ ft.position.x + 1.f, ft.position.y + 1.f });
        window.draw(shadow);

        sf::Text txt(font);
        txt.setString(toUtf8(ft.text));
        txt.setCharacterSize(15);
        txt.setFillColor(sf::Color(ft.color.r, ft.color.g, ft.color.b, a));
        txt.setPosition(ft.position);
        window.draw(txt);
    }
}

// ============================================
// ТУЛТИП ПОД МЫШЬЮ (UTF-8 РУССКИЙ)
// ============================================
void UI::drawTooltip(sf::RenderWindow& window, sf::Vector2i mousePos, const std::string& text)
{
    if (text.empty()) return;

    sf::Text txt(font);
    txt.setString(toUtf8(text));
    txt.setCharacterSize(15);
    txt.setFillColor(sf::Color(245, 245, 245));

    sf::FloatRect bounds = txt.getLocalBounds();
    float padX = 10.f;
    float padY = 6.f;
    float boxW = bounds.size.x + padX * 2.f;
    float boxH = bounds.size.y + padY * 2.f;

    float posX = static_cast<float>(mousePos.x) + 14.f;
    float posY = static_cast<float>(mousePos.y) + 14.f;

    if (posX + boxW > 800.f) posX = 800.f - boxW - 6.f;
    if (posY + boxH > 600.f) posY = 600.f - boxH - 6.f;

    sf::RectangleShape box({ boxW, boxH });
    box.setFillColor(sf::Color(20, 22, 28, 250));
    box.setOutlineColor(sf::Color(160, 130, 80));
    box.setOutlineThickness(1.5f);
    box.setPosition({ posX, posY });

    txt.setPosition({ posX + padX - bounds.position.x, posY + padY - bounds.position.y });

    window.draw(box);
    window.draw(txt);
}

// ============================================
// ПОДРОБНАЯ КАРТОЧКА ПРЕДМЕТА ПРИ ЗАЖАТИИ ПКМ
// ============================================
void UI::drawDetailedItemCard(sf::RenderWindow& window, sf::Vector2i mousePos, const ItemData& data)
{
    float cardW = 310.f;
    float cardH = 195.f;

    float posX = static_cast<float>(mousePos.x) + 16.f;
    float posY = static_cast<float>(mousePos.y) + 16.f;

    float winW = static_cast<float>(window.getSize().x);
    float winH = static_cast<float>(window.getSize().y);

    if (posX + cardW > winW - 12.f) posX = static_cast<float>(mousePos.x) - cardW - 16.f;
    if (posY + cardH > winH - 12.f) posY = winH - cardH - 12.f;
    if (posX < 12.f) posX = 12.f;
    if (posY < 12.f) posY = 12.f;

    // Фон карточки
    sf::RectangleShape bg({ cardW, cardH });
    bg.setPosition({ posX, posY });
    bg.setFillColor(sf::Color(18, 20, 26, 252));
    bg.setOutlineColor(sf::Color(215, 170, 80));
    bg.setOutlineThickness(2.5f);
    window.draw(bg);

    // Внутренняя декоративная рамка
    sf::RectangleShape innerBorder({ cardW - 8.f, cardH - 8.f });
    innerBorder.setPosition({ posX + 4.f, posY + 4.f });
    innerBorder.setFillColor(sf::Color::Transparent);
    innerBorder.setOutlineColor(sf::Color(80, 70, 55, 160));
    innerBorder.setOutlineThickness(1.f);
    window.draw(innerBorder);

    // Иконка предмета
    drawItemIcon(window, posX + 12.f, posY + 12.f, Item{ data.id, 1 });

    // Название предмета
    sf::Text nameTxt(font);
    nameTxt.setString(toUtf8(data.name));
    nameTxt.setCharacterSize(16);
    nameTxt.setFillColor(sf::Color(255, 230, 130));
    nameTxt.setPosition({ posX + 54.f, posY + 10.f });
    window.draw(nameTxt);

    // Категория и стак
    sf::Text catTxt(font);
    std::string catStr = data.category.empty() ? "Предмет" : data.category;
    catTxt.setString(toUtf8("[" + catStr + "] • Стакается: до " + std::to_string(data.maxStack)));
    catTxt.setCharacterSize(11);
    catTxt.setFillColor(sf::Color(140, 200, 240));
    catTxt.setPosition({ posX + 54.f, posY + 32.f });
    window.draw(catTxt);

    // Золотой разделитель
    sf::RectangleShape sep({ cardW - 24.f, 1.5f });
    sep.setPosition({ posX + 12.f, posY + 54.f });
    sep.setFillColor(sf::Color(160, 130, 70, 200));
    window.draw(sep);

    // Описание с переносом строк
    std::string desc = data.description.empty() ? "Обычный предмет долины." : data.description;
    std::string wrapped;
    std::string line;
    for (char c : desc)
    {
        if (c == '\n') { wrapped += line + "\n"; line.clear(); continue; }
        line += c;
        if (line.size() > 38 && c == ' ')
        {
            wrapped += line + "\n";
            line.clear();
        }
    }
    wrapped += line;

    sf::Text descTxt(font);
    descTxt.setString(toUtf8(wrapped));
    descTxt.setCharacterSize(12);
    descTxt.setFillColor(sf::Color(220, 225, 235));
    descTxt.setPosition({ posX + 12.f, posY + 62.f });
    window.draw(descTxt);

    // Эффект / действие
    if (!data.effects.empty())
    {
        sf::RectangleShape effBg({ cardW - 24.f, 26.f });
        effBg.setPosition({ posX + 12.f, posY + cardH - 34.f });
        effBg.setFillColor(sf::Color(30, 42, 35, 220));
        effBg.setOutlineColor(sf::Color(70, 120, 80));
        effBg.setOutlineThickness(1.f);
        window.draw(effBg);

        sf::Text effTxt(font);
        effTxt.setString(toUtf8(data.effects));
        effTxt.setCharacterSize(11);
        effTxt.setFillColor(sf::Color(140, 255, 160));
        effTxt.setPosition({ posX + 18.f, posY + cardH - 30.f });
        window.draw(effTxt);
    }
}

// ============================================
// ПРЕДМЕТ В РУКАХ ИГРОКА
// ============================================
void UI::drawHeldItem(sf::RenderWindow& window, const Player& player, const Item& item)
{
    if (player.isActing || item.isEmpty()) return;

    auto it = itemIcons.find(item.id);
    if (it == itemIcons.end()) return;

    sf::Sprite sprite(it->second);

    sf::Vector2f pos = player.shape.getPosition();
    float cx = pos.x + Player::SIZE / 2.f;
    float cy = pos.y + Player::SIZE / 2.f;

    // Покачивание при ходьбе
    float bob = player.isMoving ? std::sin(player.animTimer * 2.f) * 1.5f : 0.f;

    // Масштаб: аккуратные ~22x22 на экране под пропорции персонажа
    float baseScale = 22.f / 32.f;
    sprite.setOrigin({ 16.f, 16.f });

    float ix = cx;
    float iy = cy;
    float scaleX = baseScale;
    float scaleY = baseScale;
    float rot = 0.f;

    if (player.dirY > 0)
    {
        // Взгляд вниз: предмет перед игроком в руках
        ix = cx - 2.f;
        iy = cy + 4.f + bob;
        rot = -12.f;
    }
    else if (player.dirY < 0)
    {
        // Взгляд вверх: персонаж виден со спины, предмет приподнят
        ix = cx + 8.f;
        iy = cy - 2.f + bob;
        rot = 18.f;
    }
    else if (player.dirX > 0)
    {
        // Взгляд вправо: предмет в руках вперед
        ix = cx + 8.f;
        iy = cy + 3.f + bob;
        rot = 15.f;
    }
    else if (player.dirX < 0)
    {
        // Взгляд влево: предмет развернут влево
        ix = cx - 8.f;
        iy = cy + 3.f + bob;
        scaleX = -baseScale;
        rot = -15.f;
    }

    sprite.setScale({ scaleX, scaleY });
    sprite.setRotation(sf::degrees(rot));
    sprite.setPosition({ ix, iy });

    window.draw(sprite);
}

// ============================================
// ШКАЛА ЭНЕРГИИ (СТАМИНА)
// ============================================
void UI::drawEnergyBar(sf::RenderWindow& window, float currentEnergy, float maxEnergy)
{
    float barW = 18.f;
    float barH = 140.f;
    float barX = static_cast<float>(window.getSize().x) - 40.f;
    float barY = static_cast<float>(window.getSize().y) - barH - 25.f;

    // Рамка фона
    sf::RectangleShape bg({ barW, barH });
    bg.setPosition({ barX, barY });
    bg.setFillColor(sf::Color(25, 25, 30, 220));
    bg.setOutlineColor(sf::Color(160, 130, 80));
    bg.setOutlineThickness(2.f);
    window.draw(bg);

    // Заполнение снизу вверх
    float pct = std::clamp(currentEnergy / maxEnergy, 0.f, 1.f);
    float fillH = (barH - 4.f) * pct;

    if (fillH > 0.f)
    {
        sf::RectangleShape fill({ barW - 4.f, fillH });
        fill.setPosition({ barX + 2.f, barY + barH - 2.f - fillH });

        if (pct > 0.5f) fill.setFillColor(sf::Color(80, 210, 80));
        else if (pct > 0.2f) fill.setFillColor(sf::Color(230, 200, 50));
        else fill.setFillColor(sf::Color(220, 60, 50));

        window.draw(fill);
    }

    // Числовой счётчик энергии как в Stardew Valley (всегда виден над шкалой)
    std::string countStr = std::to_string(static_cast<int>(currentEnergy));
    sf::Text countText(font);
    countText.setString(countStr);
    countText.setCharacterSize(14);
    sf::FloatRect cb = countText.getLocalBounds();
    countText.setOrigin({ cb.position.x + cb.size.x / 2.f, cb.position.y + cb.size.y });
    float cx = barX + barW / 2.f;
    float cy = barY - 6.f;

    // Тонкая тень для читаемости
    countText.setFillColor(sf::Color(0, 0, 0, 220));
    countText.setPosition({ cx + 1.f, cy + 1.f });
    window.draw(countText);

    // Цвет счетчика: белый / золотистый / красный при истощении
    if (pct > 0.5f) countText.setFillColor(sf::Color(245, 245, 245));
    else if (pct > 0.2f) countText.setFillColor(sf::Color(255, 220, 90));
    else countText.setFillColor(sf::Color(255, 90, 80));
    countText.setPosition({ cx, cy });
    window.draw(countText);

    // Иконка "E" внизу под шкалой
    sf::Text eText(font);
    eText.setString("E");
    eText.setCharacterSize(14);
    eText.setFillColor(sf::Color(255, 230, 140));
    eText.setPosition({ barX + 3.f, barY + barH + 3.f });
    window.draw(eText);

    // Проверка ховера для подробного тултипа (по всей высоте шкалы и счетчика)
    sf::Vector2i mpos = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);
    if (mx >= barX - 10.f && mx <= barX + barW + 10.f && my >= barY - 25.f && my <= barY + barH + 20.f)
    {
        hoveredItemName = "Энергия: " + std::to_string(static_cast<int>(currentEnergy)) + " / " + std::to_string(static_cast<int>(maxEnergy));
    }
}

// ============================================
// МЕНЮ АЛХИМИИ И ЗЕЛЬЕВАРЕНИЯ
// ============================================
void UI::drawAlchemy(sf::RenderWindow& window, const Inventory& inv, const ItemDatabase& itemDB)
{
    float panelX = 175.f;
    float panelY = 110.f;
    float panelW = 450.f;
    float panelH = 340.f;

    // Фон окна
    sf::RectangleShape panel({ panelW, panelH });
    panel.setPosition({ panelX, panelY });
    panel.setFillColor(sf::Color(22, 24, 30, 245));
    panel.setOutlineColor(sf::Color(180, 140, 70));
    panel.setOutlineThickness(2.5f);
    window.draw(panel);

    // Заголовок
    sf::Text title(font);
    title.setString(toUtf8("Котел алхимика (Зельеварение)"));
    title.setCharacterSize(18);
    title.setFillColor(sf::Color(255, 220, 120));
    title.setPosition({ panelX + 20.f, panelY + 15.f });
    window.draw(title);

    sf::Text sub(font);
    sub.setString(toUtf8("Кликните по рецепту, чтобы сварить зелье [C - закрыть]"));
    sub.setCharacterSize(13);
    sub.setFillColor(sf::Color(170, 175, 190));
    sub.setPosition({ panelX + 20.f, panelY + 42.f });
    window.draw(sub);

    // Список рецептов
    struct Recipe {
        std::string resultId;
        std::string name;
        std::string desc;
        std::string costText;
        bool canCraft;
    };

    bool hasEnergy = (inv.countItem("berry") >= 1 && inv.countItem("fiber") >= 1);
    bool hasSpeed  = (inv.countItem("berry") >= 1 && inv.countItem("seeds") >= 2);
    bool hasGrowth = (inv.countItem("fiber") >= 2 && inv.countItem("seeds") >= 1);

    std::vector<Recipe> recipes = {
        { "energy_potion", "Зелье энергии", "+50 энергии на ПКМ", "1 Ягода + 1 Волокно", hasEnergy },
        { "speed_potion", "Зелье скорости", "+40% скорости на 30 сек", "1 Ягода + 2 Семени", hasSpeed },
        { "growth_potion", "Эликсир роста", "Ускоряет соседние грядки", "2 Волокна + 1 Семя", hasGrowth }
    };

    sf::Vector2i mpos = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    for (size_t i = 0; i < recipes.size(); i++)
    {
        float rowY = panelY + 75.f + i * 80.f;
        float rowX = panelX + 15.f;
        float rowW = panelW - 30.f;
        float rowH = 70.f;

        bool hovered = isInside(mx, my, rowX, rowY, rowW);
        if (hovered)
        {
            hoveredItemName = recipes[i].canCraft ? "Нажмите для создания зелья" : "Недостаточно ингредиентов";
        }

        sf::RectangleShape rowBg({ rowW, rowH });
        rowBg.setPosition({ rowX, rowY });
        if (recipes[i].canCraft)
        {
            rowBg.setFillColor(hovered ? sf::Color(45, 52, 45, 230) : sf::Color(32, 38, 35, 200));
            rowBg.setOutlineColor(hovered ? sf::Color(120, 220, 120) : sf::Color(80, 140, 80));
        }
        else
        {
            rowBg.setFillColor(sf::Color(28, 28, 34, 180));
            rowBg.setOutlineColor(sf::Color(65, 65, 75));
        }
        rowBg.setOutlineThickness(1.5f);
        window.draw(rowBg);

        // Иконка зелья
        drawItemIcon(window, rowX + 10.f - 9.f, rowY + 12.f - 9.f, Item{ recipes[i].resultId, 1 });

        // Название зелья
        sf::Text rName(font);
        rName.setString(toUtf8(recipes[i].name));
        rName.setCharacterSize(16);
        rName.setFillColor(recipes[i].canCraft ? sf::Color(240, 240, 240) : sf::Color(150, 150, 150));
        rName.setPosition({ rowX + 55.f, rowY + 12.f });
        window.draw(rName);

        // Описание эффекта
        sf::Text rDesc(font);
        rDesc.setString(toUtf8(recipes[i].desc));
        rDesc.setCharacterSize(12);
        rDesc.setFillColor(recipes[i].canCraft ? sf::Color(180, 215, 180) : sf::Color(130, 130, 130));
        rDesc.setPosition({ rowX + 55.f, rowY + 36.f });
        window.draw(rDesc);

        // Стоимость
        sf::Text rCost(font);
        rCost.setString(toUtf8(recipes[i].costText));
        rCost.setCharacterSize(13);
        rCost.setFillColor(recipes[i].canCraft ? sf::Color(255, 220, 130) : sf::Color(180, 90, 90));
        sf::FloatRect cb = rCost.getLocalBounds();
        rCost.setPosition({ rowX + rowW - cb.size.x - 20.f, rowY + 24.f });
        window.draw(rCost);
    }
}

// ============================================
// ВКЛАДКА КРАФТА В ИНВЕНТАРЕ
// ============================================
void UI::drawCraftingTab(sf::RenderWindow& window, const Inventory& inv, const ItemDatabase& itemDB)
{
    float startX = 235.f;
    float startY = 225.f;
    float rowW = 490.f;
    float rowH = 65.f;

    struct CraftItem {
        std::string resultId;
        std::string name;
        std::string costText;
        bool canCraft;
    };

    bool canChest = (inv.countItem("wood") >= 20);
    bool canTable = (inv.countItem("wood") >= 15 && inv.countItem("stone") >= 10);
    bool canFlask = (inv.countItem("stone") >= 1);
    bool canSeeds = (inv.countItem("berry") >= 1);

    std::vector<CraftItem> list = {
        { "chest", "Деревянный сундук (хранилище)", "20 Древесины", canChest },
        { "alchemist_table", "Стол алхимика 1-го ур. (ступка)", "15 Дерева + 10 Камня", canTable },
        { "empty_flask", "Пустой стеклянный флакон", "1 Камень", canFlask },
        { "seeds", "Пакетик семян (2 шт.)", "1 Ягода", canSeeds }
    };

    sf::Vector2i mpos = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    float itemH = 58.f;
    for (size_t i = 0; i < list.size(); i++)
    {
        float y = startY + i * (itemH + 8.f);
        bool hovered = isInsideRect(mx, my, startX, y, rowW, itemH);

        if (hovered)
        {
            hoveredItemName = list[i].canCraft ? "Кликните для создания" : "Недостаточно материалов";
        }

        sf::RectangleShape box({ rowW, itemH });
        box.setPosition({ startX, y });
        if (list[i].canCraft)
        {
            box.setFillColor(hovered ? sf::Color(45, 52, 45, 240) : sf::Color(32, 38, 35, 210));
            box.setOutlineColor(hovered ? sf::Color(120, 220, 120) : sf::Color(80, 150, 80));
        }
        else
        {
            box.setFillColor(sf::Color(28, 28, 32, 190));
            box.setOutlineColor(sf::Color(65, 65, 75));
        }
        box.setOutlineThickness(1.5f);
        window.draw(box);

        drawItemIcon(window, startX + 10.f - 9.f, y + 4.f - 9.f, Item{ list[i].resultId, 1 });

        sf::Text nameTxt(font);
        nameTxt.setString(toUtf8(list[i].name));
        nameTxt.setCharacterSize(14);
        nameTxt.setFillColor(list[i].canCraft ? sf::Color(240, 240, 240) : sf::Color(140, 140, 140));
        nameTxt.setPosition({ startX + 55.f, y + 8.f });
        window.draw(nameTxt);

        sf::Text costTxt(font);
        costTxt.setString(toUtf8("Требуется: " + list[i].costText));
        costTxt.setCharacterSize(12);
        costTxt.setFillColor(list[i].canCraft ? sf::Color(255, 215, 110) : sf::Color(180, 90, 90));
        costTxt.setPosition({ startX + 55.f, y + 30.f });
        window.draw(costTxt);
    }
}

// ============================================
// ВКЛАДКА ДНЕВНИКА В ИНВЕНТАРЕ (СТРАНИЦЫ СЛЕВА, КАТЕГОРИИ СПРАВА)
// ============================================
void UI::drawJournalTab(sf::RenderWindow& window, int selectedCategory, const AlchemySystem* alchemy)
{
    float startX = 235.f;
    float startY = 220.f;
    float pageW = 350.f;
    float pageH = 270.f;

    sf::Vector2i mpos = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    // 1. Вкладки категорий СПРАВА (закладки книги)
    const char* categoryNames[] = { "Зелья", "Растения", "Ремесло" };
    float tabX = startX + pageW + 10.f; // 595.f
    float tabW = 120.f;
    float tabH = 34.f;

    for (int c = 0; c < 3; c++)
    {
        float ty = startY + 15.f + c * 44.f;
        bool isSel = (selectedCategory == c);
        bool hovered = isInsideRect(mx, my, tabX, ty, tabW, tabH);

        sf::RectangleShape catTab({ tabW, tabH });
        catTab.setPosition({ tabX, ty });
        if (isSel)
        {
            catTab.setFillColor(sf::Color(55, 50, 65, 255));
            catTab.setOutlineColor(sf::Color(255, 215, 80));
            catTab.setOutlineThickness(2.f);
        }
        else
        {
            catTab.setFillColor(hovered ? sf::Color(40, 38, 48) : sf::Color(28, 26, 35, 220));
            catTab.setOutlineColor(sf::Color(90, 80, 70));
            catTab.setOutlineThickness(1.5f);
        }
        window.draw(catTab);

        sf::Text catTxt(font);
        catTxt.setString(toUtf8(categoryNames[c]));
        catTxt.setCharacterSize(14);
        catTxt.setFillColor(isSel ? sf::Color(255, 230, 130) : sf::Color(175, 175, 185));
        sf::FloatRect cb = catTxt.getLocalBounds();
        catTxt.setPosition({ tabX + (tabW - cb.size.x) / 2.f, ty + 8.f });
        window.draw(catTxt);
    }

    // 2. Страница СЛЕВА (пергаментная панель выбранной категории)
    sf::RectangleShape pageBg({ pageW, pageH });
    pageBg.setPosition({ startX, startY });
    pageBg.setFillColor(sf::Color(24, 24, 30, 240));
    pageBg.setOutlineColor(sf::Color(120, 95, 60));
    pageBg.setOutlineThickness(1.5f);
    window.draw(pageBg);

    // Заголовок страницы
    sf::Text header(font);
    header.setCharacterSize(16);
    header.setFillColor(sf::Color(255, 220, 110));
    header.setPosition({ startX + 12.f, startY + 10.f });

    if (selectedCategory == 0) // Зелья
    {
        header.setString(toUtf8("Книга рецептов зельеварения"));
        window.draw(header);

        if (alchemy != nullptr)
        {
            const auto& rlist = alchemy->getRecipes();
            float rowY = startY + 38.f;
            for (size_t i = 0; i < rlist.size() && i < 3; i++)
            {
                const auto& r = rlist[i];
                sf::RectangleShape rowBox({ pageW - 20.f, 65.f });
                rowBox.setPosition({ startX + 10.f, rowY });
                rowBox.setFillColor(r.discovered ? sf::Color(35, 42, 38, 220) : sf::Color(28, 28, 34, 200));
                rowBox.setOutlineColor(r.discovered ? sf::Color(100, 180, 110) : sf::Color(70, 70, 85));
                rowBox.setOutlineThickness(1.f);
                window.draw(rowBox);

                if (r.discovered)
                {
                    drawItemIcon(window, startX + 14.f, rowY + 12.f, Item{ r.resultId, 1 });

                    sf::Text rName(font);
                    rName.setString(toUtf8(r.name));
                    rName.setCharacterSize(14);
                    rName.setFillColor(sf::Color(255, 225, 120));
                    rName.setPosition({ startX + 55.f, rowY + 6.f });
                    window.draw(rName);

                    sf::Text rForm(font);
                    rForm.setString(toUtf8("Флакон с водой + " + r.formulaText));
                    rForm.setCharacterSize(11);
                    rForm.setFillColor(sf::Color(170, 225, 255));
                    rForm.setPosition({ startX + 55.f, rowY + 26.f });
                    window.draw(rForm);

                    sf::Text rEff(font);
                    rEff.setString(toUtf8(r.effectText));
                    rEff.setCharacterSize(11);
                    rEff.setFillColor(sf::Color(190, 235, 180));
                    rEff.setPosition({ startX + 55.f, rowY + 44.f });
                    window.draw(rEff);
                }
                else
                {
                    sf::Text rName(font);
                    rName.setString(toUtf8("??? Запечатанный рецепт"));
                    rName.setCharacterSize(14);
                    rName.setFillColor(sf::Color(160, 160, 175));
                    rName.setPosition({ startX + 18.f, rowY + 10.f });
                    window.draw(rName);

                    sf::Text rHint(font);
                    rHint.setString(toUtf8("Экспериментируйте с чистой водой, травами\nи порошками в алхимическом котле."));
                    rHint.setCharacterSize(11);
                    rHint.setFillColor(sf::Color(130, 130, 145));
                    rHint.setPosition({ startX + 18.f, rowY + 30.f });
                    window.draw(rHint);
                }

                rowY += 72.f;
            }
        }
    }
    else if (selectedCategory == 1) // Растения
    {
        header.setString(toUtf8("Атлас трав и растений долины"));
        window.draw(header);

        sf::Text body(font);
        body.setString(toUtf8(
            "- Дикая ягода:\n"
            "  Растет на полянах. Можно есть сырой (+15 энергии)\n"
            "  или перетереть в ступке в ягодный порошок.\n\n"
            "- Волокно и трава:\n"
            "  Сорняки после скашивания. Исходный материал для\n"
            "  травяного порошка и эликсиров роста.\n\n"
            "- Семена и грядки:\n"
            "  4 стадии роста (семя -> росток -> цвет -> урожай).\n"
            "  Поливайте грядки каждый день для ускорения роста!"
        ));
        body.setCharacterSize(12);
        body.setFillColor(sf::Color(215, 225, 220));
        body.setPosition({ startX + 12.f, startY + 38.f });
        window.draw(body);
    }
    else // Ремесло
    {
        header.setString(toUtf8("Руководство мастера и усадьба"));
        window.draw(header);

        sf::Text body(font);
        body.setString(toUtf8(
            "- Уютный дом и сон:\n"
            "  Войдите в дом и нажмите [E] у кровати, чтобы\n"
            "  выспаться (восстанавливает 100% энергии).\n"
            "  ВНИМАНИЕ: не задерживайтесь до 02:00, иначе\n"
            "  потеряете сознание от усталости!\n\n"
            "- Алхимический стол (1-й уровень):\n"
            "  Каменная ступка позволяет перетирать ягоды и листья\n"
            "  в тончайшие порошки высшей очистки.\n\n"
            "- Алхимический котел:\n"
            "  Поместите флакон с водой в центр и точные\n"
            "  ингредиенты вокруг. Неверные рецепты сгорают!"
        ));
        body.setCharacterSize(12);
        body.setFillColor(sf::Color(230, 225, 210));
        body.setPosition({ startX + 12.f, startY + 36.f });
        window.draw(body);
    }
}

// ============================================
// ИНТЕРАКТИВНАЯ АЛХИМИЧЕСКАЯ СТАНЦИЯ (КОТЕЛ)
// ============================================
void UI::drawInteractiveCauldron(sf::RenderWindow& window,
    const Inventory& inv,
    const AlchemySystem& alchemy,
    const ItemDatabase& itemDB)
{
    const auto& ingredients = alchemy.cauldronIngredients;
    const auto& base = alchemy.cauldronBase;
    bool isBrewing = alchemy.isBrewing;
    float brewTimer = alchemy.brewTimer;

    float winX = 60.f;
    float winY = 60.f;
    float winW = 680.f;
    float winH = 480.f;

    // Рамка окна
    sf::RectangleShape winBg({ winW, winH });
    winBg.setPosition({ winX, winY });
    winBg.setFillColor(sf::Color(20, 22, 28, 250));
    winBg.setOutlineColor(sf::Color(180, 140, 70));
    winBg.setOutlineThickness(2.5f);
    window.draw(winBg);

    // Заголовок
    sf::Text title(font);
    title.setString(toUtf8("Котел алхимика"));
    title.setCharacterSize(20);
    title.setFillColor(sf::Color(255, 225, 125));
    title.setPosition({ winX + 25.f, winY + 15.f });
    window.draw(title);

    sf::Text sub(font);
    sub.setString(toUtf8("Кликайте на предметы слева, чтобы добавить в котел [E/ESC - закрыть]"));
    sub.setCharacterSize(13);
    sub.setFillColor(sf::Color(170, 175, 190));
    sub.setPosition({ winX + 25.f, winY + 45.f });
    window.draw(sub);

    sf::Vector2i mpos = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    // Кнопка рецептов
    float recBtnX = winX + 510.f;
    float recBtnY = winY + 16.f;
    float recBtnW = 145.f;
    float recBtnH = 34.f;
    bool hovRec = isInside(mx, my, recBtnX, recBtnY, recBtnW);
    sf::RectangleShape recBtn({ recBtnW, recBtnH });
    recBtn.setPosition({ recBtnX, recBtnY });
    recBtn.setFillColor(hovRec ? sf::Color(75, 55, 105) : sf::Color(50, 36, 75));
    recBtn.setOutlineColor(hovRec ? sf::Color(255, 215, 110) : sf::Color(180, 140, 70));
    recBtn.setOutlineThickness(1.5f);
    window.draw(recBtn);

    sf::Text recTxt(font);
    recTxt.setString(toUtf8("Рецепты [R]"));
    recTxt.setCharacterSize(14);
    recTxt.setFillColor(sf::Color(255, 235, 150));
    recTxt.setPosition({ recBtnX + 28.f, recBtnY + 8.f });
    window.draw(recTxt);

    // === ЛЕВАЯ ЧАСТЬ: ИНВЕНТАРЬ (4x9 слотов) ===
    float invX = winX + 20.f;
    float invY = winY + 80.f;
    float slotSize = 40.f;
    float slotPad = 4.f;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float sx = invX + col * (slotSize + slotPad);
            float sy = invY + row * (slotSize + slotPad);

            int slotIdx = (row == 3) ? col : (9 + row * 9 + col);
            const Item& item = inv.getSlot(slotIdx);

            bool hov = isInside(mx, my, sx, sy, slotSize);
            if (hov && !item.isEmpty())
            {
                const ItemData* d = itemDB.find(item.id);
                if (d) hoveredItemName = d->name + " (Кликните, чтобы положить в котел)";
            }

            sf::RectangleShape s({ slotSize, slotSize });
            s.setPosition({ sx, sy });
            s.setFillColor(hov ? sf::Color(60, 65, 80) : sf::Color(38, 38, 45));
            s.setOutlineColor(hov ? sf::Color(140, 200, 255) : sf::Color(70, 70, 80));
            s.setOutlineThickness(1.f);
            window.draw(s);

            if (!item.isEmpty())
            {
                drawItemIcon(window, sx + (slotSize - 32.f) / 2.f - 9.f, sy + (slotSize - 32.f) / 2.f - 9.f, item);
            }
        }
    }

    // Разделитель
    sf::RectangleShape sep({ 2.f, 360.f });
    sep.setPosition({ winX + 430.f, winY + 80.f });
    sep.setFillColor(sf::Color(80, 75, 70));
    window.draw(sep);

    // === ПРАВАЯ ЧАСТЬ: КОТЕЛ И КРУГ СЛОТОВ ===
    float cauldronCenterX = winX + 555.f;
    float cauldronCenterY = winY + 235.f;

    // Большой котел в центре
    sf::CircleShape cauldronBody(44.f);
    cauldronBody.setOrigin({ 44.f, 44.f });
    cauldronBody.setPosition({ cauldronCenterX, cauldronCenterY + 10.f });
    cauldronBody.setFillColor(sf::Color(45, 45, 55));
    cauldronBody.setOutlineColor(sf::Color(75, 80, 95));
    cauldronBody.setOutlineThickness(4.f);
    window.draw(cauldronBody);

    // Бурлящая жидкость внутри котла
    sf::CircleShape liquid(36.f);
    liquid.setOrigin({ 36.f, 36.f });
    liquid.setPosition({ cauldronCenterX, cauldronCenterY + 10.f });
    liquid.setFillColor(isBrewing ? sf::Color(220, 80, 240) : sf::Color(100, 45, 170));
    window.draw(liquid);

    // 6 слотов ингредиентов вокруг котла по кругу
    float radius = 85.f;
    for (int i = 0; i < 6; i++)
    {
        float angle = i * (3.14159265f / 3.f) - 3.14159265f / 2.f;
        float sx = cauldronCenterX + std::cos(angle) * radius - 20.f;
        float sy = cauldronCenterY + std::sin(angle) * radius - 20.f;

        // Линия связи с котлом
        sf::Vertex line[] = {
            sf::Vertex({ cauldronCenterX, cauldronCenterY }, sf::Color(140, 100, 220, 100)),
            sf::Vertex({ sx + 20.f, sy + 20.f }, sf::Color(140, 100, 220, 100))
        };
        window.draw(line, 2, sf::PrimitiveType::Lines);

        bool hov = isInside(mx, my, sx, sy, 40.f);
        if (hov && !ingredients[i].isEmpty())
        {
            const ItemData* d = itemDB.find(ingredients[i].id);
            if (d) hoveredItemName = d->name + " (Кликните, чтобы забрать)";
        }

        // Стеклянная колбочка-ячейка для ингредиента (вместо абстрактных кружков)
        // 1. Деревянная подставка под колбочку
        sf::RectangleShape stand({ 28.f, 4.f });
        stand.setPosition({ sx + 6.f, sy + 38.f });
        stand.setFillColor(sf::Color(80, 70, 60));
        stand.setOutlineColor(sf::Color(120, 100, 80));
        stand.setOutlineThickness(1.f);
        window.draw(stand);

        // 2. Стеклянная колбочка (округлое тело колбы)
        sf::CircleShape flaskBulb(16.f);
        flaskBulb.setPosition({ sx + 4.f, sy + 10.f });
        flaskBulb.setFillColor(hov ? sf::Color(50, 75, 105, 190) : sf::Color(25, 35, 50, 160));
        flaskBulb.setOutlineColor(hov ? sf::Color(255, 225, 110) : sf::Color(140, 210, 255, 210));
        flaskBulb.setOutlineThickness(1.5f);
        window.draw(flaskBulb);

        // 3. Стеклянное горлышко колбы
        sf::RectangleShape flaskNeck({ 8.f, 12.f });
        flaskNeck.setPosition({ sx + 16.f, sy });
        flaskNeck.setFillColor(hov ? sf::Color(50, 75, 105, 190) : sf::Color(25, 35, 50, 160));
        flaskNeck.setOutlineColor(hov ? sf::Color(255, 225, 110) : sf::Color(140, 210, 255, 210));
        flaskNeck.setOutlineThickness(1.5f);
        window.draw(flaskNeck);

        // 4. Пробка на горлышке
        sf::RectangleShape flaskCork({ 12.f, 4.f });
        flaskCork.setPosition({ sx + 14.f, sy - 2.f });
        flaskCork.setFillColor(sf::Color(170, 130, 90));
        window.draw(flaskCork);

        // 5. Блик света на стекле колбочки
        sf::RectangleShape highlight({ 2.f, 10.f });
        highlight.setPosition({ sx + 9.f, sy + 15.f });
        highlight.setFillColor(sf::Color(255, 255, 255, 170));
        window.draw(highlight);

        if (!ingredients[i].isEmpty())
        {
            drawItemIcon(window, sx + 4.f - 9.f, sy + 6.f - 9.f, ingredients[i]);
        }
    }

    // Центральный слот базы (большая алхимическая колба с эссенцией)
    float baseSlotX = cauldronCenterX - 20.f;
    float baseSlotY = cauldronCenterY - 10.f;
    bool hovBase = isInside(mx, my, baseSlotX, baseSlotY, 40.f);
    if (hovBase)
    {
        if (base.isEmpty()) hoveredItemName = "Колба основы (поместите флакон с чистой водой)";
        else {
            const ItemData* d = itemDB.find(base.id);
            if (d) hoveredItemName = d->name + " (Основа варки)";
        }
    }

    // Центральная подставка под колбу базы
    sf::RectangleShape centralStand({ 44.f, 5.f });
    centralStand.setPosition({ baseSlotX - 2.f, baseSlotY + 40.f });
    centralStand.setFillColor(sf::Color(90, 75, 50));
    centralStand.setOutlineColor(sf::Color(180, 140, 70));
    centralStand.setOutlineThickness(1.5f);
    window.draw(centralStand);

    // Стеклянное тело центральной колбы
    sf::CircleShape baseBulb(20.f);
    baseBulb.setPosition({ baseSlotX, baseSlotY });
    baseBulb.setFillColor(hovBase ? sf::Color(75, 55, 95, 210) : sf::Color(35, 28, 50, 190));
    baseBulb.setOutlineColor(hovBase ? sf::Color(255, 220, 100) : sf::Color(210, 150, 255));
    baseBulb.setOutlineThickness(2.f);
    window.draw(baseBulb);

    // Горлышко центральной колбы
    sf::RectangleShape baseNeck({ 10.f, 14.f });
    baseNeck.setPosition({ baseSlotX + 15.f, baseSlotY - 12.f });
    baseNeck.setFillColor(hovBase ? sf::Color(75, 55, 95, 210) : sf::Color(35, 28, 50, 190));
    baseNeck.setOutlineColor(hovBase ? sf::Color(255, 220, 100) : sf::Color(210, 150, 255));
    baseNeck.setOutlineThickness(1.5f);
    window.draw(baseNeck);

    // Пробка центральной колбы
    sf::RectangleShape baseCork({ 14.f, 4.f });
    baseCork.setPosition({ baseSlotX + 13.f, baseSlotY - 15.f });
    baseCork.setFillColor(sf::Color(180, 140, 90));
    window.draw(baseCork);

    if (!base.isEmpty())
    {
        drawItemIcon(window, baseSlotX + 4.f - 9.f, baseSlotY + 4.f - 9.f, base);
    }
    else
    {
        sf::Text hint(font);
        hint.setString(toUtf8("Флакон"));
        hint.setCharacterSize(10);
        hint.setFillColor(sf::Color(180, 180, 200));
        hint.setPosition({ baseSlotX + 2.f, baseSlotY + 12.f });
        window.draw(hint);
    }

    // Анимация бурления во время варки
    if (isBrewing)
    {
        for (int b = 0; b < 6; b++)
        {
            float bx = cauldronCenterX - 25.f + (b * 11) % 50;
            float by = cauldronCenterY - 15.f - std::fmod(brewTimer * 60.f + b * 15.f, 50.f);
            sf::CircleShape bubble(3.f + (b % 3));
            bubble.setPosition({ bx, by });
            bubble.setFillColor(sf::Color(240, 180, 255, 200));
            window.draw(bubble);
        }
    }

    // Кнопка "Сварить зелье"
    float btnX = winX + 475.f;
    float btnY = winY + 400.f;
    float btnW = 160.f;
    float btnH = 42.f;

    bool hovBtn = isInside(mx, my, btnX, btnY, btnW);
    sf::RectangleShape btn({ btnW, btnH });
    btn.setPosition({ btnX, btnY });
    btn.setFillColor(hovBtn ? sf::Color(55, 95, 60) : sf::Color(40, 70, 45));
    btn.setOutlineColor(hovBtn ? sf::Color(140, 255, 140) : sf::Color(90, 170, 90));
    btn.setOutlineThickness(2.f);
    window.draw(btn);

    sf::Text btnTxt(font);
    btnTxt.setString(toUtf8(isBrewing ? "Идет варка..." : "Сварить зелье"));
    btnTxt.setCharacterSize(15);
    btnTxt.setFillColor(sf::Color::White);
    sf::FloatRect bb = btnTxt.getLocalBounds();
    btnTxt.setPosition({ btnX + (btnW - bb.size.x) / 2.f, btnY + 10.f });
    window.draw(btnTxt);

    // Всплывающая книга рецептов в котле
    if (alchemy.showRecipes)
    {
        drawCauldronRecipeBook(window, winX, winY, alchemy, itemDB);
    }
}

// ============================================
// ВСПЛЫВАЮЩЕЕ ОКНО РЕЦЕПТОВ В КОТЛЕ
// ============================================
void UI::drawCauldronRecipeBook(sf::RenderWindow& window, float winX, float winY, const AlchemySystem& alchemy, const ItemDatabase& itemDB)
{
    float bookW = 460.f;
    float bookH = 410.f;
    float bx = winX + 110.f;
    float by = winY + 35.f;

    // Фоновая подложка
    sf::RectangleShape bookBg({ bookW, bookH });
    bookBg.setPosition({ bx, by });
    bookBg.setFillColor(sf::Color(22, 24, 32, 252));
    bookBg.setOutlineColor(sf::Color(225, 185, 80));
    bookBg.setOutlineThickness(3.f);
    window.draw(bookBg);

    // Декоративная внутренняя рамка
    sf::RectangleShape inner({ bookW - 8.f, bookH - 8.f });
    inner.setPosition({ bx + 4.f, by + 4.f });
    inner.setFillColor(sf::Color::Transparent);
    inner.setOutlineColor(sf::Color(70, 60, 45, 180));
    inner.setOutlineThickness(1.f);
    window.draw(inner);

    // Заголовок
    sf::Text title(font);
    title.setString(toUtf8("Рецепты алхимического котла"));
    title.setCharacterSize(18);
    title.setFillColor(sf::Color(255, 230, 130));
    title.setPosition({ bx + 22.f, by + 14.f });
    window.draw(title);

    // Кнопка [X]
    sf::Vector2i mpos = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    float cx = bx + bookW - 36.f;
    float cy = by + 12.f;
    bool hovClose = isInside(mx, my, cx, cy, 26.f);

    sf::RectangleShape closeBtn({ 26.f, 26.f });
    closeBtn.setPosition({ cx, cy });
    closeBtn.setFillColor(hovClose ? sf::Color(220, 50, 50) : sf::Color(170, 40, 40));
    closeBtn.setOutlineColor(sf::Color(255, 100, 100));
    closeBtn.setOutlineThickness(1.5f);
    window.draw(closeBtn);

    sf::Text closeTxt(font);
    closeTxt.setString("X");
    closeTxt.setCharacterSize(14);
    closeTxt.setFillColor(sf::Color::White);
    closeTxt.setPosition({ cx + 7.f, cy + 4.f });
    window.draw(closeTxt);

    // Список рецептов
    const auto& rlist = alchemy.getRecipes();
    float rowY = by + 50.f;

    for (size_t i = 0; i < rlist.size() && i < 3; i++)
    {
        const auto& r = rlist[i];
        sf::RectangleShape rowBox({ bookW - 30.f, 105.f });
        rowBox.setPosition({ bx + 15.f, rowY });
        rowBox.setFillColor(r.discovered ? sf::Color(32, 36, 46, 230) : sf::Color(25, 27, 34, 210));
        rowBox.setOutlineColor(r.discovered ? sf::Color(90, 130, 105) : sf::Color(65, 65, 80));
        rowBox.setOutlineThickness(1.5f);
        window.draw(rowBox);

        if (r.discovered)
        {
            // Иконка получаемого зелья
            drawItemIcon(window, bx + 24.f, rowY + 16.f, Item{ r.resultId, 1 });

            // Название зелья
            sf::Text rName(font);
            rName.setString(toUtf8(r.name));
            rName.setCharacterSize(15);
            rName.setFillColor(sf::Color(255, 230, 130));
            rName.setPosition({ bx + 68.f, rowY + 10.f });
            window.draw(rName);

            // Формула варки
            sf::Text rForm(font);
            rForm.setString(toUtf8("Основа: Флакон с водой\nИнгредиенты: " + r.formulaText));
            rForm.setCharacterSize(11);
            rForm.setFillColor(sf::Color(150, 220, 255));
            rForm.setPosition({ bx + 68.f, rowY + 34.f });
            window.draw(rForm);

            // Эффект
            sf::Text rEff(font);
            rEff.setString(toUtf8("Эффект: " + r.effectText));
            rEff.setCharacterSize(11);
            rEff.setFillColor(sf::Color(140, 255, 160));
            rEff.setPosition({ bx + 68.f, rowY + 76.f });
            window.draw(rEff);
        }
        else
        {
            // Заблокированный рецепт
            sf::RectangleShape lockBox({ 32.f, 32.f });
            lockBox.setPosition({ bx + 24.f, rowY + 16.f });
            lockBox.setFillColor(sf::Color(38, 40, 50));
            lockBox.setOutlineColor(sf::Color(70, 75, 90));
            lockBox.setOutlineThickness(1.f);
            window.draw(lockBox);

            sf::Text qmark(font);
            qmark.setString("?");
            qmark.setCharacterSize(18);
            qmark.setFillColor(sf::Color(150, 150, 170));
            qmark.setPosition({ bx + 34.f, rowY + 20.f });
            window.draw(qmark);

            sf::Text rName(font);
            rName.setString(toUtf8("??? Запечатанный рецепт"));
            rName.setCharacterSize(14);
            rName.setFillColor(sf::Color(160, 160, 175));
            rName.setPosition({ bx + 68.f, rowY + 12.f });
            window.draw(rName);

            sf::Text rHint(font);
            rHint.setString(toUtf8("Формула не разгадана.\nЭкспериментируйте с чистой водой и травами в котле!"));
            rHint.setCharacterSize(11);
            rHint.setFillColor(sf::Color(130, 130, 145));
            rHint.setPosition({ bx + 68.f, rowY + 36.f });
            window.draw(rHint);

            sf::Text rLock(font);
            rLock.setString(toUtf8("Сварите зелье хотя бы один раз, чтобы открыть запись."));
            rLock.setCharacterSize(10);
            rLock.setFillColor(sf::Color(170, 145, 90));
            rLock.setPosition({ bx + 68.f, rowY + 76.f });
            window.draw(rLock);
        }

        rowY += 114.f;
    }
}

// ============================================
// СТОЛ АЛХИМИКА: СТУПКА И ПЕСТИК
// ============================================
void UI::drawMortarStation(sf::RenderWindow& window,
    const Inventory& inv,
    const AlchemySystem& alchemy,
    const ItemDatabase& itemDB)
{
    float winX = 60.f;
    float winY = 60.f;
    float winW = 780.f;
    float winH = 480.f;

    // Фон станции
    sf::RectangleShape bg({ winW, winH });
    bg.setPosition({ winX, winY });
    bg.setFillColor(sf::Color(22, 22, 28, 250));
    bg.setOutlineColor(sf::Color(165, 125, 65));
    bg.setOutlineThickness(3.f);
    window.draw(bg);

    // Заголовок
    sf::Text title(font);
    title.setString(toUtf8("Стол алхимика: Каменная ступка"));
    title.setCharacterSize(20);
    title.setFillColor(sf::Color(255, 220, 110));
    title.setPosition({ winX + 25.f, winY + 20.f });
    window.draw(title);

    // Подзаголовок
    sf::Text subtitle(font);
    subtitle.setString(toUtf8("Перетирайте ягоды и растения в порошки для зелий"));
    subtitle.setCharacterSize(13);
    subtitle.setFillColor(sf::Color(180, 180, 195));
    subtitle.setPosition({ winX + 25.f, winY + 48.f });
    window.draw(subtitle);

    // Кнопка закрытия [X]
    sf::RectangleShape closeBtn({ 32.f, 32.f });
    closeBtn.setPosition({ winX + winW - 45.f, winY + 20.f });
    closeBtn.setFillColor(sf::Color(65, 30, 30));
    closeBtn.setOutlineColor(sf::Color(180, 80, 80));
    closeBtn.setOutlineThickness(1.5f);
    window.draw(closeBtn);

    sf::Text closeTxt(font);
    closeTxt.setString("X");
    closeTxt.setCharacterSize(18);
    closeTxt.setFillColor(sf::Color::White);
    closeTxt.setPosition({ winX + winW - 35.f, winY + 24.f });
    window.draw(closeTxt);

    // Сетка инвентаря слева
    float invX = winX + 25.f;
    float invY = winY + 90.f;
    float slotSize = 40.f;
    float slotPad = 4.f;

    sf::Vector2i mpos = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float sx = invX + col * (slotSize + slotPad);
            float sy = invY + row * (slotSize + slotPad);

            int slotIdx = (row == 3) ? col : (9 + row * 9 + col);
            const Item& item = inv.getSlot(slotIdx);

            bool hov = isInside(mx, my, sx, sy, slotSize);
            if (hov && !item.isEmpty())
            {
                const ItemData* d = itemDB.find(item.id);
                if (d) hoveredItemName = d->name + " (Кликните для ступки)";
            }

            sf::RectangleShape s({ slotSize, slotSize });
            s.setPosition({ sx, sy });
            s.setFillColor(hov ? sf::Color(60, 65, 80) : sf::Color(38, 38, 45));
            s.setOutlineColor(hov ? sf::Color(140, 200, 255) : sf::Color(70, 70, 80));
            s.setOutlineThickness(1.f);
            window.draw(s);

            if (!item.isEmpty())
            {
                drawItemIcon(window, sx + (slotSize - 32.f) / 2.f - 9.f, sy + (slotSize - 32.f) / 2.f - 9.f, item);
            }
        }
    }

    // Разделитель
    sf::RectangleShape sep({ 2.f, 340.f });
    sep.setPosition({ winX + 430.f, winY + 90.f });
    sep.setFillColor(sf::Color(80, 75, 70));
    window.draw(sep);

    // Правая часть: Ступка и слоты
    float rightCenterX = winX + 600.f;

    // Входной слот (Input)
    float inX = rightCenterX - 110.f;
    float inY = winY + 180.f;
    sf::RectangleShape inSlot({ 50.f, 50.f });
    inSlot.setPosition({ inX, inY });
    inSlot.setFillColor(sf::Color(45, 42, 50));
    inSlot.setOutlineColor(sf::Color(180, 150, 90));
    inSlot.setOutlineThickness(2.f);
    window.draw(inSlot);

    sf::Text inLabel(font);
    inLabel.setString(toUtf8("Сырье"));
    inLabel.setCharacterSize(12);
    inLabel.setFillColor(sf::Color(200, 200, 200));
    inLabel.setPosition({ inX + 8.f, inY - 20.f });
    window.draw(inLabel);

    if (!alchemy.mortarInput.isEmpty())
    {
        drawItemIcon(window, inX - 9.f, inY - 9.f, alchemy.mortarInput);
    }

    // Стрелочка ->
    sf::Text arrow(font);
    arrow.setString("==>");
    arrow.setCharacterSize(20);
    arrow.setFillColor(sf::Color(220, 190, 120));
    arrow.setPosition({ rightCenterX - 25.f, inY + 12.f });
    window.draw(arrow);

    // Выходной слот (Output)
    float outX = rightCenterX + 60.f;
    float outY = winY + 180.f;
    sf::RectangleShape outSlot({ 50.f, 50.f });
    outSlot.setPosition({ outX, outY });
    outSlot.setFillColor(sf::Color(38, 48, 40));
    outSlot.setOutlineColor(sf::Color(100, 200, 120));
    outSlot.setOutlineThickness(2.f);
    window.draw(outSlot);

    sf::Text outLabel(font);
    outLabel.setString(toUtf8("Порошок"));
    outLabel.setCharacterSize(12);
    outLabel.setFillColor(sf::Color(160, 230, 170));
    outLabel.setPosition({ outX + 2.f, outY - 20.f });
    window.draw(outLabel);

    if (!alchemy.mortarOutput.isEmpty())
    {
        drawItemIcon(window, outX - 9.f, outY - 9.f, alchemy.mortarOutput);
    }

    // Прогресс-бар перетирания
    if (alchemy.isGrinding)
    {
        float barW = 200.f;
        float barH = 16.f;
        float barX = rightCenterX - 100.f;
        float barY = inY + 80.f;

        sf::RectangleShape barBg({ barW, barH });
        barBg.setPosition({ barX, barY });
        barBg.setFillColor(sf::Color(30, 30, 35));
        barBg.setOutlineColor(sf::Color(100, 100, 110));
        barBg.setOutlineThickness(1.f);
        window.draw(barBg);

        float progress = alchemy.grindTimer / AlchemySystem::GRIND_DURATION;
        sf::RectangleShape barFill({ barW * progress, barH });
        barFill.setPosition({ barX, barY });
        barFill.setFillColor(sf::Color(240, 160, 60));
        window.draw(barFill);
    }

    // Кнопка "Перетереть"
    float btnX = rightCenterX - 90.f;
    float btnY = winY + 330.f;
    float btnW = 180.f;
    float btnH = 44.f;

    bool canDo = alchemy.canGrind() && !alchemy.isGrinding;
    bool hovBtn = isInside(mx, my, btnX, btnY, btnW);

    sf::RectangleShape grindBtn({ btnW, btnH });
    grindBtn.setPosition({ btnX, btnY });
    grindBtn.setFillColor(canDo ? (hovBtn ? sf::Color(160, 100, 45) : sf::Color(130, 80, 35)) : sf::Color(55, 55, 60));
    grindBtn.setOutlineColor(canDo ? sf::Color(255, 200, 100) : sf::Color(80, 80, 85));
    grindBtn.setOutlineThickness(2.f);
    window.draw(grindBtn);

    sf::Text grindTxt(font);
    grindTxt.setString(toUtf8(alchemy.isGrinding ? "Измельчение..." : "Перетереть в порошок"));
    grindTxt.setCharacterSize(14);
    grindTxt.setFillColor(canDo ? sf::Color::White : sf::Color(140, 140, 140));
    sf::FloatRect gb = grindTxt.getLocalBounds();
    grindTxt.setPosition({ btnX + (btnW - gb.size.x) / 2.f, btnY + 12.f });
    window.draw(grindTxt);
}
