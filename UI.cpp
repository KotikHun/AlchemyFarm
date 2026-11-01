#include "UI.hpp"
#include "Player.hpp"
#include "AlchemySystem.hpp"
#include "SaveManager.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

static bool isInside(float mx, float my, float x, float y, float size)
{
    return mx >= x && mx < x + size && my >= y && my < y + size;
}

static bool isInsideRect(float mx, float my, float x, float y, float w, float h)
{
    return mx >= x && mx < x + w && my >= y && my < y + h;
}

static std::string wrapString(const sf::Font& font, const std::string& input, unsigned int charSize, float maxW)
{
    sf::Text tester(font);
    tester.setCharacterSize(charSize);
    std::istringstream stream(input);
    std::string word;
    std::string result;
    std::string currentLine;

    while (stream >> word)
    {
        std::string testLine = currentLine.empty() ? word : (currentLine + " " + word);
        tester.setString(toUtf8(testLine));
        if (tester.getLocalBounds().size.x > maxW && !currentLine.empty())
        {
            if (!result.empty()) result += "\n";
            result += currentLine;
            currentLine = word;
        }
        else
        {
            currentLine = testLine;
        }
    }
    if (!currentLine.empty())
    {
        if (!result.empty()) result += "\n";
        result += currentLine;
    }
    return result;
}

static sf::Vector2i getMousePos(const sf::RenderWindow& window)
{
    sf::Vector2f c = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    return sf::Vector2i(static_cast<int>(std::round(c.x)), static_cast<int>(std::round(c.y)));
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
    float hotbarX = (static_cast<float>(window.getView().getSize().x) - totalHotbarWidth) / 2.f;
    float hotbarY = static_cast<float>(window.getView().getSize().y) - SLOT_SIZE - 20.f;

    hotbarBg.setSize({ totalHotbarWidth, hotbarHeight });
    hotbarBg.setPosition({ hotbarX, hotbarY - SLOT_PADDING });
    window.draw(hotbarBg);

    sf::Vector2i mousePixel = getMousePos(window);
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
void UI::drawHUD(sf::RenderWindow& window, int currentDay, const std::string& timeStr, bool isCurfew, int hour, int gold)
{
    float hudW = 156.f;
    float hudH = 68.f;
    float hudX = static_cast<float>(window.getView().getSize().x) - hudW - 20.f;
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
    timeTxt.setCharacterSize(15);
    // При комендантском часе (после 00:00) время горит красным, предупреждая игрока!
    timeTxt.setFillColor(isCurfew ? sf::Color(255, 65, 65) : sf::Color(240, 240, 245));
    timeTxt.setPosition({ hudX + 12.f, hudY + 35.f });
    window.draw(timeTxt);

    // Значок времени дня (солнце / луна)
    sf::CircleShape sun(6.f);
    sun.setPosition({ hudX + 66.f, hudY + 39.f });
    if (hour >= 6 && hour < 19) {
        sun.setFillColor(sf::Color(255, 215, 60)); // Солнце
    } else {
        sun.setFillColor(sf::Color(160, 200, 255)); // Луна
    }
    window.draw(sun);

    // Золотая монетка
    sf::CircleShape coin(6.f);
    coin.setPosition({ hudX + 88.f, hudY + 39.f });
    coin.setFillColor(sf::Color(255, 215, 0));
    coin.setOutlineColor(sf::Color(180, 140, 30));
    coin.setOutlineThickness(1.f);
    window.draw(coin);

    // Монеты / золото игрока
    sf::Text goldTxt(font);
    goldTxt.setString(std::to_string(gold) + "g");
    goldTxt.setCharacterSize(14);
    goldTxt.setFillColor(sf::Color(255, 225, 90));
    goldTxt.setStyle(sf::Text::Bold);
    goldTxt.setPosition({ hudX + 104.f, hudY + 35.f });
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
        sf::Vector2i mpos = getMousePos(window);
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

    sf::Vector2i mousePixel = getMousePos(window);
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
    const ItemDatabase& itemDB,
    const std::string& customTitle)
{
    if (!isOpen) return;

    bool isShipping = (customTitle.find("сбыта") != std::string::npos || customTitle.find("продаж") != std::string::npos);

    sf::RectangleShape chestBg({ 510.f, 155.f });
    chestBg.setFillColor(sf::Color(30, 25, 22, 245));
    chestBg.setOutlineColor(isShipping ? sf::Color(255, 215, 80) : sf::Color(160, 110, 60));
    chestBg.setOutlineThickness(2.f);
    chestBg.setPosition({ 225.f, 130.f });
    window.draw(chestBg);

    sf::Text title(font);
    title.setString(toUtf8(customTitle.empty() ? "Сундук" : customTitle));
    title.setCharacterSize(15);
    title.setFillColor(isShipping ? sf::Color(255, 230, 110) : sf::Color(255, 220, 150));
    title.setPosition({ 235.f, 138.f });
    window.draw(title);

    const float GRID_X = 235.f;
    const float GRID_Y = 165.f;

    sf::Vector2i mousePixel = getMousePos(window);
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

    float centerX = static_cast<float>(window.getView().getSize().x) / 2.f;
    float posY = static_cast<float>(window.getView().getSize().y) - SLOT_SIZE - 20.f - 24.f;

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

    float winW = static_cast<float>(window.getView().getSize().x);
    float winH = static_cast<float>(window.getView().getSize().y);

    if (posX + boxW > winW - 6.f) posX = static_cast<float>(mousePos.x) - boxW - 8.f;
    if (posY + boxH > winH - 6.f) posY = static_cast<float>(mousePos.y) - boxH - 8.f;
    if (posX < 6.f) posX = 6.f;
    if (posY < 6.f) posY = 6.f;

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

    float winW = static_cast<float>(window.getView().getSize().x);
    float winH = static_cast<float>(window.getView().getSize().y);

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

    // Категория, стак и цена продажи
    sf::Text catTxt(font);
    std::string catStr = data.category.empty() ? "Предмет" : data.category;
    std::string infoStr = "[" + catStr + "] • Стак: до " + std::to_string(data.maxStack);
    if (data.sellPrice > 0)
    {
        infoStr += " • Продажа: " + std::to_string(data.sellPrice) + " G";
    }
    catTxt.setString(toUtf8(infoStr));
    catTxt.setCharacterSize(11);
    catTxt.setFillColor(data.sellPrice > 0 ? sf::Color(255, 225, 120) : sf::Color(140, 200, 240));
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
    float barX = static_cast<float>(window.getView().getSize().x) - 40.f;
    float barY = static_cast<float>(window.getView().getSize().y) - barH - 25.f;

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
    sf::Vector2i mpos = getMousePos(window);
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

    sf::Vector2i mpos = getMousePos(window);
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
// ВСПОМОГАТЕЛЬНЫЕ ПРЯМОУГОЛЬНИКИ КРАФТА
// ============================================
sf::FloatRect UI::getCraftingRecipeRect(int index)
{
    float startX = 235.f;
    float startY = 222.f;
    float colW = 240.f;
    float rowH = 56.f;
    float gapY = 8.f;
    float gapX = 10.f;

    int col = (index >= 4) ? 1 : 0;
    int row = (index >= 4) ? (index - 4) : index;

    float x = startX + col * (colW + gapX);
    float y = startY + row * (rowH + gapY);
    return sf::FloatRect({ x, y }, { colW, rowH });
}

// ============================================
// ВКЛАДКА КРАФТА В ИНВЕНТАРЕ
// ============================================
void UI::drawCraftingTab(sf::RenderWindow& window, const Inventory& inv, const ItemDatabase& itemDB)
{
    struct CraftItem {
        std::string resultId;
        std::string name;
        std::string costText;
        bool canCraft;
    };

    bool canChest = (inv.countItem("wood") >= 20);
    bool canTable = (inv.countItem("wood") >= 15 && inv.countItem("stone") >= 10);
    bool canCauldron = (inv.countItem("stone") >= 15 && inv.countItem("wood") >= 5);
    bool canBed = (inv.countItem("wood") >= 25 && inv.countItem("fiber") >= 10);
    bool canCarpet = (inv.countItem("fiber") >= 12);
    bool canFlask = (inv.countItem("stone") >= 1);
    bool canSeeds = (inv.countItem("berry") >= 1);

    std::vector<CraftItem> list = {
        { "chest", "Сундук (хранилище)", "20 Дерева", canChest },
        { "alchemist_table", "Стол алхимика (ступка)", "15 Дер. + 10 Кам.", canTable },
        { "cauldron", "Алхимический котел", "15 Кам. + 5 Дер.", canCauldron },
        { "bed", "Уютная кровать", "25 Дер. + 10 Вол.", canBed },
        { "carpet", "Мягкий ковер", "12 Волокна", canCarpet },
        { "empty_flask", "Пустой стекл. флакон", "1 Камень", canFlask },
        { "seeds", "Пакетик семян (2 шт.)", "1 Ягода", canSeeds }
    };

    sf::Vector2i mpos = getMousePos(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    for (size_t i = 0; i < list.size(); i++)
    {
        sf::FloatRect r = getCraftingRecipeRect(static_cast<int>(i));
        bool hovered = r.contains({ mx, my });

        if (hovered)
        {
            hoveredItemName = list[i].canCraft ? "Кликните для создания" : "Недостаточно материалов";
        }

        sf::RectangleShape box({ r.size.x, r.size.y });
        box.setPosition(r.position);
        if (list[i].canCraft)
        {
            box.setFillColor(hovered ? sf::Color(45, 55, 45, 240) : sf::Color(32, 40, 35, 215));
            box.setOutlineColor(hovered ? sf::Color(120, 230, 120) : sf::Color(80, 160, 85));
        }
        else
        {
            box.setFillColor(sf::Color(28, 28, 32, 190));
            box.setOutlineColor(sf::Color(65, 65, 75));
        }
        box.setOutlineThickness(1.5f);
        window.draw(box);

        drawItemIcon(window, r.position.x + 8.f - 9.f, r.position.y + 4.f - 9.f, Item{ list[i].resultId, 1 });

        sf::Text nameTxt(font);
        nameTxt.setString(toUtf8(list[i].name));
        nameTxt.setCharacterSize(13);
        nameTxt.setFillColor(list[i].canCraft ? sf::Color(240, 240, 240) : sf::Color(140, 140, 140));
        nameTxt.setPosition({ r.position.x + 50.f, r.position.y + 6.f });
        window.draw(nameTxt);

        sf::Text costTxt(font);
        costTxt.setString(toUtf8("Нужно: " + list[i].costText));
        costTxt.setCharacterSize(11);
        costTxt.setFillColor(list[i].canCraft ? sf::Color(255, 215, 110) : sf::Color(180, 90, 90));
        costTxt.setPosition({ r.position.x + 50.f, r.position.y + 28.f });
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

    sf::Vector2i mpos = getMousePos(window);
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
            float rowY = startY + 36.f;
            for (size_t i = 0; i < rlist.size() && i < 3; i++)
            {
                const auto& r = rlist[i];
                sf::RectangleShape rowBox({ pageW - 20.f, 68.f });
                rowBox.setPosition({ startX + 10.f, rowY });
                rowBox.setFillColor(r.discovered ? sf::Color(35, 42, 38, 220) : sf::Color(28, 28, 34, 200));
                rowBox.setOutlineColor(r.discovered ? sf::Color(100, 180, 110) : sf::Color(70, 70, 85));
                rowBox.setOutlineThickness(1.f);
                window.draw(rowBox);

                if (r.discovered)
                {
                    drawItemIcon(window, startX + 14.f, rowY + 14.f, Item{ r.resultId, 1 });

                    sf::Text rName(font);
                    rName.setString(toUtf8(r.name));
                    rName.setCharacterSize(13);
                    rName.setFillColor(sf::Color(255, 225, 120));
                    rName.setPosition({ startX + 54.f, rowY + 6.f });
                    window.draw(rName);

                    sf::Text rForm(font);
                    rForm.setString(toUtf8("Рецепт: " + r.formulaText));
                    rForm.setCharacterSize(11);
                    rForm.setFillColor(sf::Color(170, 225, 255));
                    rForm.setPosition({ startX + 54.f, rowY + 26.f });
                    window.draw(rForm);

                    sf::Text rEff(font);
                    rEff.setString(toUtf8("Эффект: " + r.effectText));
                    rEff.setCharacterSize(11);
                    rEff.setFillColor(sf::Color(190, 235, 180));
                    rEff.setPosition({ startX + 54.f, rowY + 44.f });
                    window.draw(rEff);
                }
                else
                {
                    sf::Text rName(font);
                    rName.setString(toUtf8("??? Запечатанный рецепт"));
                    rName.setCharacterSize(13);
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

                rowY += 74.f;
            }
        }
    }
    else if (selectedCategory == 1) // Растения
    {
        header.setString(toUtf8("Атлас трав, культур и погоды"));
        window.draw(header);

        sf::Text body(font);
        body.setString(toUtf8(
            "- Дикая ягода:\n"
            "  Растет на полянках фермы. Дает +15 энергии или\n"
            "  перетирается в ступке в ягодный порошок.\n\n"
            "- Волокно и трава:\n"
            "  Сорняки после скашивания. Исходный материал для\n"
            "  травяного порошка, ковров и мягких кроватей.\n\n"
            "- Семена и грядки (4 стадии созревания):\n"
            "  Поливайте грядки лейкой для роста каждый день!\n"
            "  * В дождливую погоду дождь автоматически\n"
            "    поливает все вспаханные грядки на ферме!"
        ));
        body.setCharacterSize(11);
        body.setFillColor(sf::Color(215, 225, 220));
        body.setPosition({ startX + 12.f, startY + 34.f });
        window.draw(body);
    }
    else // Ремесло
    {
        header.setString(toUtf8("Руководство мастера и усадьба"));
        window.draw(header);

        sf::Text body(font);
        body.setString(toUtf8(
            "- Уютный дом и сон:\n"
            "  Нажмите [E] у кровати, чтобы восстановить силы.\n"
            "  Ложитесь до 02:00, чтобы избежать обморока!\n\n"
            "- Доска объявлений и заказов:\n"
            "  Выполняйте ежедневные заказы жителей долины за\n"
            "  золото и покупайте ценные материалы в лавке.\n\n"
            "- Ящик сбыта товаров (Shipping Bin):\n"
            "  Складывайте урожай и зелья — ночью они\n"
            "  автоматически продаются с подробным отчетом!\n\n"
            "- Котел и ступка:\n"
            "  Измельчайте травы и варите чудодейственные зелья."
        ));
        body.setCharacterSize(11);
        body.setFillColor(sf::Color(230, 225, 210));
        body.setPosition({ startX + 12.f, startY + 34.f });
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

    sf::Vector2i mpos = getMousePos(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    // Кнопка рецептов
    float recBtnX = winX + 510.f;
    float recBtnY = winY + 16.f;
    float recBtnW = 145.f;
    float recBtnH = 34.f;
    bool hovRec = isInsideRect(mx, my, recBtnX, recBtnY, recBtnW, recBtnH);
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
            if (ingredients[i].count > 1)
            {
                sf::Text countText(font);
                countText.setString(std::to_string(ingredients[i].count));
                countText.setCharacterSize(12);
                countText.setFillColor(sf::Color::White);
                countText.setOutlineColor(sf::Color::Black);
                countText.setOutlineThickness(1.2f);
                countText.setPosition({ sx + 22.f, sy + 22.f });
                window.draw(countText);
            }
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
        if (base.count > 1)
        {
            sf::Text countText(font);
            countText.setString(std::to_string(base.count));
            countText.setCharacterSize(12);
            countText.setFillColor(sf::Color::White);
            countText.setOutlineColor(sf::Color::Black);
            countText.setOutlineThickness(1.2f);
            countText.setPosition({ baseSlotX + 24.f, baseSlotY + 22.f });
            window.draw(countText);
        }
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

    bool hovBtn = isInsideRect(mx, my, btnX, btnY, btnW, btnH);
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
    sf::Vector2i mpos = getMousePos(window);
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

    // Список только открытых (разгаданных) рецептов
    std::vector<const PotionRecipe*> discoveredList;
    for (const auto& r : alchemy.getRecipes())
    {
        if (r.discovered)
        {
            discoveredList.push_back(&r);
        }
    }

    if (discoveredList.empty())
    {
        sf::Text emptyTitle(font);
        emptyTitle.setString(toUtf8("Нет открытых рецептов"));
        emptyTitle.setCharacterSize(16);
        emptyTitle.setFillColor(sf::Color(220, 190, 120));
        emptyTitle.setPosition({ bx + 120.f, by + 120.f });
        window.draw(emptyTitle);

        sf::Text emptyDesc(font);
        emptyDesc.setString(toUtf8("Рецепты пока не разгаданы.\n\nЭкспериментируйте у котла:\nналивайте в центр чистую воду,\nпробуйте класть травы, ягоды, семена\nили порошки в ячейки вокруг!\n\nУспешная варка навсегда запишет\nновый рецепт в эту книгу."));
        emptyDesc.setCharacterSize(13);
        emptyDesc.setFillColor(sf::Color(160, 160, 175));
        emptyDesc.setPosition({ bx + 70.f, by + 160.f });
        window.draw(emptyDesc);
    }
    else
    {
        float rowY = by + 50.f;
        for (size_t i = 0; i < discoveredList.size() && i < 3; i++)
        {
            const auto& r = *discoveredList[i];
            sf::RectangleShape rowBox({ bookW - 30.f, 105.f });
            rowBox.setPosition({ bx + 15.f, rowY });
            rowBox.setFillColor(sf::Color(32, 36, 46, 230));
            rowBox.setOutlineColor(sf::Color(90, 130, 105));
            rowBox.setOutlineThickness(1.5f);
            window.draw(rowBox);

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

            rowY += 114.f;
        }
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

    sf::Vector2i mpos = getMousePos(window);
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
    bool hovBtn = isInsideRect(mx, my, btnX, btnY, btnW, btnH);

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

// ============================================
// ГЛАВНОЕ МЕНЮ И ЭКРАНЫ ЗАГРУЗКИ / ПАУЗЫ
// ============================================

sf::FloatRect UI::getMainMenuButtonRect(int index)
{
    float btnW = 260.f;
    float btnH = 50.f;
    float btnX = 350.f;
    float startY = 270.f;
    float spacing = 65.f;
    return sf::FloatRect({ btnX, startY + index * spacing }, { btnW, btnH });
}

sf::FloatRect UI::getPauseMenuButtonRect(int index)
{
    float btnW = 250.f;
    float btnH = 46.f;
    float btnX = 355.f;
    float startY = 238.f;
    float spacing = 58.f;
    return sf::FloatRect({ btnX, startY + index * spacing }, { btnW, btnH });
}

sf::FloatRect UI::getAboutBackButtonRect()
{
    float winX = 200.f;
    float winY = 120.f;
    float winW = 560.f;
    float winH = 530.f;
    float backW = 180.f;
    float backH = 42.f;
    float backX = winX + (winW - backW) / 2.f;
    float backY = winY + winH - 56.f;
    return sf::FloatRect({ backX, backY }, { backW, backH });
}

sf::FloatRect UI::getSettingsBackButtonRect()
{
    float winX = 220.f;
    float winY = 120.f;
    float winW = 520.f;
    float winH = 560.f;
    float backW = 180.f;
    float backH = 42.f;
    float backX = winX + (winW - backW) / 2.f;
    float backY = winY + winH - 62.f;
    return sf::FloatRect({ backX, backY }, { backW, backH });
}

sf::FloatRect UI::getSettingsSoundMinusRect()
{
    float winX = 220.f;
    float winY = 120.f;
    return sf::FloatRect({ winX + 280.f, winY + 66.f }, { 32.f, 28.f });
}

sf::FloatRect UI::getSettingsSoundPlusRect()
{
    float winX = 220.f;
    float winY = 120.f;
    return sf::FloatRect({ winX + 385.f, winY + 66.f }, { 32.f, 28.f });
}

sf::FloatRect UI::getSettingsMusicMinusRect()
{
    float winX = 220.f;
    float winY = 120.f;
    return sf::FloatRect({ winX + 280.f, winY + 111.f }, { 32.f, 28.f });
}

sf::FloatRect UI::getSettingsMusicPlusRect()
{
    float winX = 220.f;
    float winY = 120.f;
    return sf::FloatRect({ winX + 385.f, winY + 111.f }, { 32.f, 28.f });
}

sf::FloatRect UI::getSavesBackButtonRect()
{
    float winW = 580.f;
    float winH = 540.f;
    float winX = (960.f - winW) / 2.f;
    float winY = (800.f - winH) / 2.f;
    float backW = 180.f;
    float backH = 40.f;
    float backX = winX + (winW - backW) / 2.f;
    float backY = winY + winH - 52.f;
    return sf::FloatRect({ backX, backY }, { backW, backH });
}

sf::FloatRect UI::getSavesActionBtnRect(int slotIndex)
{
    float winW = 580.f;
    float winH = 540.f;
    float winX = (960.f - winW) / 2.f;
    float winY = (800.f - winH) / 2.f;
    float slotY = winY + 82.f + slotIndex * 116.f;
    float slotW = winW - 40.f;
    float slotH = 98.f;
    float slotX = winX + 20.f;
    float btn1W = 100.f;
    float btn1H = 36.f;
    float btn1X = slotX + slotW - 200.f;
    float btn1Y = slotY + (slotH - btn1H) / 2.f;
    return sf::FloatRect({ btn1X, btn1Y }, { btn1W, btn1H });
}

sf::FloatRect UI::getSavesDeleteBtnRect(int slotIndex)
{
    float winW = 580.f;
    float winH = 540.f;
    float winX = (960.f - winW) / 2.f;
    float winY = (800.f - winH) / 2.f;
    float slotY = winY + 82.f + slotIndex * 116.f;
    float slotW = winW - 40.f;
    float slotH = 98.f;
    float slotX = winX + 20.f;
    float delW = 80.f;
    float delH = 36.f;
    float delX = slotX + slotW - 90.f;
    float delY = slotY + (slotH - delH) / 2.f;
    return sf::FloatRect({ delX, delY }, { delW, delH });
}

sf::FloatRect UI::getSavesEmptyBtnRect(int slotIndex)
{
    float winW = 580.f;
    float winH = 540.f;
    float winX = (960.f - winW) / 2.f;
    float winY = (800.f - winH) / 2.f;
    float slotY = winY + 82.f + slotIndex * 116.f;
    float slotW = winW - 40.f;
    float slotH = 98.f;
    float slotX = winX + 20.f;
    float btnW = 120.f;
    float btnH = 36.f;
    float btnX = slotX + slotW - 135.f;
    float btnY = slotY + (slotH - btnH) / 2.f;
    return sf::FloatRect({ btnX, btnY }, { btnW, btnH });
}

void UI::drawMainMenu(sf::RenderWindow& window,
    float animTime,
    int hoveredButton,
    MenuModal modal,
    const GameSettings& settings,
    const std::vector<SaveSlotInfo>& saveSlots,
    bool isSaveMode,
    int pendingDeleteSlot,
    const std::vector<MenuParticle>& particles)
{
    float winW = static_cast<float>(window.getView().getSize().x);
    float winH = static_cast<float>(window.getView().getSize().y);

    // 1. Атмосферный градиентный фон (сумерки / рассвет)
    sf::VertexArray bgGrad(sf::PrimitiveType::TriangleStrip, 4);
    bgGrad[0].position = { 0.f, 0.f };
    bgGrad[0].color = sf::Color(16, 18, 28);
    bgGrad[1].position = { winW, 0.f };
    bgGrad[1].color = sf::Color(20, 22, 36);
    bgGrad[2].position = { 0.f, winH };
    bgGrad[2].color = sf::Color(48, 30, 42);
    bgGrad[3].position = { winW, winH };
    bgGrad[3].color = sf::Color(55, 34, 38);
    window.draw(bgGrad);

    // Мягкое золотистое свечение на горизонте
    sf::RectangleShape horizonGlow({ winW, 140.f });
    horizonGlow.setPosition({ 0.f, winH - 140.f });
    horizonGlow.setFillColor(sf::Color(90, 55, 35, 75));
    window.draw(horizonGlow);

    // 2. Парящие магические частицы
    for (const auto& p : particles)
    {
        sf::CircleShape part(p.size);
        part.setPosition(p.pos);
        sf::Color c = p.color;
        float alphaFactor = std::sin((p.life / p.maxLife) * 3.14159f);
        if (alphaFactor < 0.f) alphaFactor = 0.f;
        c.a = static_cast<std::uint8_t>(p.alpha * alphaFactor);
        part.setFillColor(c);
        window.draw(part);
    }

    // 3. Заголовок игры (Title Logo)
    float titleY = 75.f;

    // Тень заголовка
    sf::Text shadowTitle(font);
    shadowTitle.setString("ALCHEMY FARM");
    shadowTitle.setCharacterSize(44);
    shadowTitle.setFillColor(sf::Color(10, 8, 14, 220));
    shadowTitle.setStyle(sf::Text::Bold);
    sf::FloatRect tb = shadowTitle.getLocalBounds();
    shadowTitle.setPosition({ (winW - tb.size.x) / 2.f + 3.f, titleY + 3.f });
    window.draw(shadowTitle);

    // Основной текст заголовка с переливающимся золотом
    sf::Text mainTitle(font);
    mainTitle.setString("ALCHEMY FARM");
    mainTitle.setCharacterSize(44);
    float glowVal = 0.5f + 0.5f * std::sin(animTime * 2.f);
    std::uint8_t goldR = static_cast<std::uint8_t>(245 + 10 * glowVal);
    std::uint8_t goldG = static_cast<std::uint8_t>(210 + 35 * glowVal);
    std::uint8_t goldB = static_cast<std::uint8_t>(90 + 30 * glowVal);
    mainTitle.setFillColor(sf::Color(goldR, goldG, goldB));
    mainTitle.setStyle(sf::Text::Bold);
    mainTitle.setPosition({ (winW - tb.size.x) / 2.f, titleY });
    window.draw(mainTitle);

    // Подзаголовок
    sf::Text subTitle(font);
    subTitle.setString(toUtf8("✦  Магическая Ферма & Зельеварение  ✦"));
    subTitle.setCharacterSize(16);
    subTitle.setFillColor(sf::Color(185, 230, 205));
    sf::FloatRect sb = subTitle.getLocalBounds();
    subTitle.setPosition({ (winW - sb.size.x) / 2.f, titleY + 58.f });
    window.draw(subTitle);

    // Декоративная линия
    sf::Text decoLine(font);
    decoLine.setString(toUtf8("◇ ─────── ✧ ─────── ✦ ─────── ✧ ─────── ◇"));
    decoLine.setCharacterSize(14);
    decoLine.setFillColor(sf::Color(215, 170, 80, 160));
    sf::FloatRect db = decoLine.getLocalBounds();
    decoLine.setPosition({ (winW - db.size.x) / 2.f, titleY + 88.f });
    window.draw(decoLine);

    // 4. Главная панель кнопок
    float panelX = 320.f;
    float panelY = 215.f;
    float panelW = 320.f;
    float panelH = 465.f;

    sf::RectangleShape panelBg({ panelW, panelH });
    panelBg.setPosition({ panelX, panelY });
    panelBg.setFillColor(sf::Color(22, 18, 16, 235));
    panelBg.setOutlineColor(sf::Color(160, 120, 65));
    panelBg.setOutlineThickness(2.5f);
    window.draw(panelBg);

    sf::RectangleShape innerBorder({ panelW - 10.f, panelH - 10.f });
    innerBorder.setPosition({ panelX + 5.f, panelY + 5.f });
    innerBorder.setFillColor(sf::Color::Transparent);
    innerBorder.setOutlineColor(sf::Color(215, 170, 80, 80));
    innerBorder.setOutlineThickness(1.f);
    window.draw(innerBorder);

    // Заголовок панели
    sf::Text menuHeader(font);
    menuHeader.setString(toUtf8("ГЛАВНОЕ МЕНЮ"));
    menuHeader.setCharacterSize(14);
    menuHeader.setFillColor(sf::Color(210, 195, 160));
    sf::FloatRect mhb = menuHeader.getLocalBounds();
    menuHeader.setPosition({ panelX + (panelW - mhb.size.x) / 2.f, panelY + 16.f });
    window.draw(menuHeader);

    // Список кнопок
    const std::vector<std::string> buttonLabels = {
        "Новая игра",
        "Загрузить игру",
        "Настройки",
        "Об игре",
        "Выход"
    };

    for (int i = 0; i < static_cast<int>(buttonLabels.size()); i++)
    {
        sf::FloatRect r = getMainMenuButtonRect(i);
        bool isHovered = (hoveredButton == i);

        sf::RectangleShape btn({ r.size.x, r.size.y });
        btn.setPosition({ r.position.x, r.position.y });

        if (isHovered)
        {
            btn.setFillColor(sf::Color(78, 54, 34, 255));
            btn.setOutlineColor(sf::Color(255, 220, 80));
            btn.setOutlineThickness(2.5f);
        }
        else
        {
            btn.setFillColor(sf::Color(38, 28, 20, 225));
            btn.setOutlineColor(sf::Color(140, 105, 65));
            btn.setOutlineThickness(2.f);
        }
        window.draw(btn);

        // Текст кнопки
        sf::Text btnText(font);
        std::string label = buttonLabels[i];
        if (isHovered)
        {
            label = "✦  " + label + "  ✦";
        }
        btnText.setString(toUtf8(label));
        btnText.setCharacterSize(17);
        btnText.setFillColor(isHovered ? sf::Color(255, 255, 225) : sf::Color(225, 210, 185));
        sf::FloatRect b = btnText.getLocalBounds();
        btnText.setPosition({ r.position.x + (r.size.x - b.size.x) / 2.f, r.position.y + 13.f });
        window.draw(btnText);
    }

    // Подсказка внизу панели
    sf::Text promptText(font);
    promptText.setString(toUtf8("Выберите пункт меню для продолжения"));
    promptText.setCharacterSize(12);
    promptText.setFillColor(sf::Color(165, 150, 130, 170));
    sf::FloatRect pb = promptText.getLocalBounds();
    promptText.setPosition({ panelX + (panelW - pb.size.x) / 2.f, panelY + panelH - 32.f });
    window.draw(promptText);

    // Нижний футер
    sf::Text footer(font);
    footer.setString(toUtf8("Alchemy Farm v0.3.5 dev • Разработано KotikHun • SFML 3 & C++20"));
    footer.setCharacterSize(12);
    footer.setFillColor(sf::Color(150, 150, 170, 160));
    sf::FloatRect fb = footer.getLocalBounds();
    footer.setPosition({ (winW - fb.size.x) / 2.f, winH - 28.f });
    window.draw(footer);

    // 5. Модальные окна (если открыты)
    if (modal != MenuModal::None)
    {
        sf::RectangleShape darkOverlay({ winW, winH });
        darkOverlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(darkOverlay);

        if (modal == MenuModal::Settings)
        {
            drawSettingsModal(window, settings, hoveredButton);
        }
        else if (modal == MenuModal::Saves)
        {
            drawSavesModal(window, saveSlots, isSaveMode, pendingDeleteSlot);
        }
        else if (modal == MenuModal::About)
        {
            drawAboutModal(window, hoveredButton);
        }
    }
}

void UI::drawLoadingScreen(sf::RenderWindow& window,
    float progress,
    const std::string& stageText,
    const std::string& tipText,
    float animTime)
{
    float winW = static_cast<float>(window.getView().getSize().x);
    float winH = static_cast<float>(window.getView().getSize().y);

    // Фон загрузки
    sf::VertexArray bgGrad(sf::PrimitiveType::TriangleStrip, 4);
    bgGrad[0].position = { 0.f, 0.f };
    bgGrad[0].color = sf::Color(12, 14, 22);
    bgGrad[1].position = { winW, 0.f };
    bgGrad[1].color = sf::Color(18, 18, 30);
    bgGrad[2].position = { 0.f, winH };
    bgGrad[2].color = sf::Color(24, 20, 32);
    bgGrad[3].position = { winW, winH };
    bgGrad[3].color = sf::Color(32, 24, 38);
    window.draw(bgGrad);

    // Анимированная алхимическая колба / котел в центре
    float iconX = winW / 2.f;
    float iconY = 240.f;

    // Свечение колбы
    float pulse = 0.5f + 0.5f * std::sin(animTime * 3.f);
    sf::CircleShape aura(46.f + 4.f * pulse);
    aura.setOrigin({ aura.getRadius(), aura.getRadius() });
    aura.setPosition({ iconX, iconY });
    aura.setFillColor(sf::Color(80, 220, 140, static_cast<std::uint8_t>(40 + 35 * pulse)));
    window.draw(aura);

    // Рисуем котел/колбу
    sf::CircleShape cauldronPot(34.f);
    cauldronPot.setOrigin({ 34.f, 34.f });
    cauldronPot.setPosition({ iconX, iconY });
    cauldronPot.setFillColor(sf::Color(35, 35, 45));
    cauldronPot.setOutlineColor(sf::Color(215, 170, 80));
    cauldronPot.setOutlineThickness(2.5f);
    window.draw(cauldronPot);

    // Зелье внутри
    sf::CircleShape potionLiquid(22.f);
    potionLiquid.setOrigin({ 22.f, 22.f });
    potionLiquid.setPosition({ iconX, iconY + 5.f });
    potionLiquid.setFillColor(sf::Color(70, 210, 120));
    window.draw(potionLiquid);

    // Пузырьки пара над котлом
    for (int i = 0; i < 4; i++)
    {
        float bubblePhase = std::fmod(animTime * 1.5f + i * 0.4f, 1.0f);
        float bx = iconX - 14.f + i * 9.f + std::sin(animTime * 4.f + i) * 4.f;
        float by = iconY - 24.f - bubblePhase * 35.f;
        sf::CircleShape bubble(3.f * (1.f - bubblePhase * 0.4f));
        bubble.setPosition({ bx, by });
        bubble.setFillColor(sf::Color(140, 255, 180, static_cast<std::uint8_t>(220 * (1.f - bubblePhase))));
        window.draw(bubble);
    }

    // Заголовок загрузки
    sf::Text loadTitle(font);
    loadTitle.setString(toUtf8("ПОДГОТОВКА К ПРИКЛЮЧЕНИЮ"));
    loadTitle.setCharacterSize(24);
    loadTitle.setFillColor(sf::Color(255, 225, 130));
    loadTitle.setStyle(sf::Text::Bold);
    sf::FloatRect tb = loadTitle.getLocalBounds();
    loadTitle.setPosition({ (winW - tb.size.x) / 2.f, 310.f });
    window.draw(loadTitle);

    // Текст текущего этапа
    sf::Text stageTxt(font);
    stageTxt.setString(toUtf8(stageText));
    stageTxt.setCharacterSize(16);
    stageTxt.setFillColor(sf::Color(210, 235, 215));
    sf::FloatRect sb = stageTxt.getLocalBounds();
    stageTxt.setPosition({ (winW - sb.size.x) / 2.f, 360.f });
    window.draw(stageTxt);

    // Полоса прогресса
    float barW = 560.f;
    float barH = 28.f;
    float barX = (winW - barW) / 2.f;
    float barY = 410.f;

    sf::RectangleShape barFrame({ barW, barH });
    barFrame.setPosition({ barX, barY });
    barFrame.setFillColor(sf::Color(18, 16, 22, 240));
    barFrame.setOutlineColor(sf::Color(190, 145, 75));
    barFrame.setOutlineThickness(2.5f);
    window.draw(barFrame);

    float clProgress = progress;
    if (clProgress < 0.f) clProgress = 0.f;
    if (clProgress > 1.f) clProgress = 1.f;

    if (clProgress > 0.01f)
    {
        sf::RectangleShape barFill({ (barW - 6.f) * clProgress, barH - 6.f });
        barFill.setPosition({ barX + 3.f, barY + 3.f });
        barFill.setFillColor(sf::Color(65, 195, 110));
        window.draw(barFill);

        // Блик на полосе
        sf::RectangleShape barShine({ (barW - 6.f) * clProgress, 4.f });
        barShine.setPosition({ barX + 3.f, barY + 4.f });
        barShine.setFillColor(sf::Color(170, 255, 195, 160));
        window.draw(barShine);
    }

    // Процент загрузки
    int percent = static_cast<int>(clProgress * 100.f);
    sf::Text pctTxt(font);
    pctTxt.setString(std::to_string(percent) + "%");
    pctTxt.setCharacterSize(15);
    pctTxt.setFillColor(sf::Color(255, 255, 240));
    pctTxt.setStyle(sf::Text::Bold);
    sf::FloatRect pb = pctTxt.getLocalBounds();
    pctTxt.setPosition({ barX + (barW - pb.size.x) / 2.f, barY + 4.f });
    window.draw(pctTxt);

    // Карточка с советом внизу
    float tipBoxW = 640.f;
    float tipBoxH = 90.f;
    float tipBoxX = (winW - tipBoxW) / 2.f;
    float tipBoxY = 530.f;

    sf::RectangleShape tipBox({ tipBoxW, tipBoxH });
    tipBox.setPosition({ tipBoxX, tipBoxY });
    tipBox.setFillColor(sf::Color(22, 20, 24, 230));
    tipBox.setOutlineColor(sf::Color(140, 110, 65));
    tipBox.setOutlineThickness(1.5f);
    window.draw(tipBox);

    sf::Text tipHeader(font);
    tipHeader.setString(toUtf8("💡 СОВЕТ АЛХИМИКА"));
    tipHeader.setCharacterSize(13);
    tipHeader.setFillColor(sf::Color(255, 215, 80));
    tipHeader.setPosition({ tipBoxX + 20.f, tipBoxY + 12.f });
    window.draw(tipHeader);

    sf::Text tipBody(font);
    tipBody.setString(toUtf8(tipText));
    tipBody.setCharacterSize(14);
    tipBody.setFillColor(sf::Color(235, 230, 220));
    tipBody.setPosition({ tipBoxX + 20.f, tipBoxY + 38.f });
    window.draw(tipBody);

    // Подсказка пропуска
    sf::Text skipHint(font);
    skipHint.setString(toUtf8("(Нажмите ПРОБЕЛ или ЛКМ для мгновенного входа в игру)"));
    skipHint.setCharacterSize(12);
    skipHint.setFillColor(sf::Color(160, 170, 190, 180));
    sf::FloatRect shb = skipHint.getLocalBounds();
    skipHint.setPosition({ (winW - shb.size.x) / 2.f, tipBoxY + tipBoxH + 20.f });
    window.draw(skipHint);
}

void UI::drawPauseMenu(sf::RenderWindow& window,
    int hoveredButton,
    MenuModal modal,
    const GameSettings& settings,
    const std::vector<SaveSlotInfo>& saveSlots,
    bool isSaveMode,
    int pendingDeleteSlot)
{
    float winW = static_cast<float>(window.getView().getSize().x);
    float winH = static_cast<float>(window.getView().getSize().y);

    // Затемнение игрового мира
    sf::RectangleShape darkBg({ winW, winH });
    darkBg.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(darkBg);

    float panelW = 320.f;
    float panelH = 460.f;
    float panelX = (winW - panelW) / 2.f;
    float panelY = (winH - panelH) / 2.f;

    sf::RectangleShape panel({ panelW, panelH });
    panel.setPosition({ panelX, panelY });
    panel.setFillColor(sf::Color(20, 18, 16, 245));
    panel.setOutlineColor(sf::Color(215, 170, 80));
    panel.setOutlineThickness(2.5f);
    window.draw(panel);

    sf::Text title(font);
    title.setString(toUtf8("ПАУЗА"));
    title.setCharacterSize(26);
    title.setFillColor(sf::Color(255, 220, 120));
    title.setStyle(sf::Text::Bold);
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition({ panelX + (panelW - tb.size.x) / 2.f, panelY + 20.f });
    window.draw(title);

    const std::vector<std::string> pauseLabels = {
        "Продолжить",
        "Сохранить игру",
        "Загрузить игру",
        "Настройки",
        "В главное меню",
        "Выйти из игры"
    };

    for (int i = 0; i < static_cast<int>(pauseLabels.size()); i++)
    {
        sf::FloatRect r = getPauseMenuButtonRect(i);
        bool isHovered = (hoveredButton == i);

        sf::RectangleShape btn({ r.size.x, r.size.y });
        btn.setPosition({ r.position.x, r.position.y });

        if (isHovered)
        {
            btn.setFillColor(sf::Color(78, 54, 34, 255));
            btn.setOutlineColor(sf::Color(255, 220, 80));
            btn.setOutlineThickness(2.5f);
        }
        else
        {
            btn.setFillColor(sf::Color(38, 28, 20, 225));
            btn.setOutlineColor(sf::Color(140, 105, 65));
            btn.setOutlineThickness(2.f);
        }
        window.draw(btn);

        sf::Text txt(font);
        std::string label = pauseLabels[i];
        if (isHovered) label = "✦  " + label + "  ✦";
        txt.setString(toUtf8(label));
        txt.setCharacterSize(15);
        txt.setFillColor(isHovered ? sf::Color(255, 255, 225) : sf::Color(225, 210, 185));
        sf::FloatRect b = txt.getLocalBounds();
        txt.setPosition({ r.position.x + (r.size.x - b.size.x) / 2.f, r.position.y + 12.f });
        window.draw(txt);
    }

    if (modal != MenuModal::None)
    {
        if (modal == MenuModal::Settings)
        {
            drawSettingsModal(window, settings, hoveredButton);
        }
        else if (modal == MenuModal::Saves)
        {
            drawSavesModal(window, saveSlots, isSaveMode, pendingDeleteSlot);
        }
    }
}

void UI::drawSettingsModal(sf::RenderWindow& window,
    const GameSettings& settings,
    int hoveredButton)
{
    float winX = 220.f;
    float winY = 120.f;
    float winW = 520.f;
    float winH = 560.f;

    sf::RectangleShape bg({ winW, winH });
    bg.setPosition({ winX, winY });
    bg.setFillColor(sf::Color(22, 19, 18, 250));
    bg.setOutlineColor(sf::Color(215, 170, 80));
    bg.setOutlineThickness(2.5f);
    window.draw(bg);

    // Заголовок
    sf::Text header(font);
    header.setString(toUtf8("НАСТРОЙКИ"));
    header.setCharacterSize(22);
    header.setFillColor(sf::Color(255, 220, 120));
    header.setStyle(sf::Text::Bold);
    sf::FloatRect hb = header.getLocalBounds();
    header.setPosition({ winX + (winW - hb.size.x) / 2.f, winY + 18.f });
    window.draw(header);

    // Линия
    sf::RectangleShape line({ winW - 40.f, 1.5f });
    line.setPosition({ winX + 20.f, winY + 54.f });
    line.setFillColor(sf::Color(140, 105, 65));
    window.draw(line);

    // 1. Громкость звуков
    sf::Text soundLbl(font);
    soundLbl.setString(toUtf8("Звуковые эффекты:"));
    soundLbl.setCharacterSize(15);
    soundLbl.setFillColor(sf::Color(230, 220, 205));
    soundLbl.setPosition({ winX + 30.f, winY + 70.f });
    window.draw(soundLbl);

    // Кнопка [ - ]
    sf::FloatRect sndMinR = getSettingsSoundMinusRect();
    sf::RectangleShape sndMin(sndMinR.size);
    sndMin.setPosition(sndMinR.position);
    sndMin.setFillColor(sf::Color(50, 36, 26));
    sndMin.setOutlineColor(sf::Color(160, 120, 70));
    sndMin.setOutlineThickness(1.5f);
    window.draw(sndMin);

    sf::Text sndMinTxt(font);
    sndMinTxt.setString("-");
    sndMinTxt.setCharacterSize(18);
    sndMinTxt.setFillColor(sf::Color::White);
    sndMinTxt.setPosition({ sndMinR.position.x + 11.f, sndMinR.position.y + 2.f });
    window.draw(sndMinTxt);

    // Значение звука
    sf::Text sndVal(font);
    sndVal.setString(std::to_string(settings.soundVolume) + "%");
    sndVal.setCharacterSize(15);
    sndVal.setFillColor(sf::Color(255, 225, 140));
    sndVal.setPosition({ winX + 328.f, winY + 70.f });
    window.draw(sndVal);

    // Кнопка [ + ]
    sf::FloatRect sndPlsR = getSettingsSoundPlusRect();
    sf::RectangleShape sndPls(sndPlsR.size);
    sndPls.setPosition(sndPlsR.position);
    sndPls.setFillColor(sf::Color(50, 36, 26));
    sndPls.setOutlineColor(sf::Color(160, 120, 70));
    sndPls.setOutlineThickness(1.5f);
    window.draw(sndPls);

    sf::Text sndPlsTxt(font);
    sndPlsTxt.setString("+");
    sndPlsTxt.setCharacterSize(18);
    sndPlsTxt.setFillColor(sf::Color::White);
    sndPlsTxt.setPosition({ sndPlsR.position.x + 9.f, sndPlsR.position.y + 2.f });
    window.draw(sndPlsTxt);

    // 2. Громкость музыки
    sf::Text musLbl(font);
    musLbl.setString(toUtf8("Музыка:"));
    musLbl.setCharacterSize(15);
    musLbl.setFillColor(sf::Color(230, 220, 205));
    musLbl.setPosition({ winX + 30.f, winY + 115.f });
    window.draw(musLbl);

    // Кнопка [ - ]
    sf::FloatRect musMinR = getSettingsMusicMinusRect();
    sf::RectangleShape musMin(musMinR.size);
    musMin.setPosition(musMinR.position);
    musMin.setFillColor(sf::Color(50, 36, 26));
    musMin.setOutlineColor(sf::Color(160, 120, 70));
    musMin.setOutlineThickness(1.5f);
    window.draw(musMin);

    sf::Text musMinTxt(font);
    musMinTxt.setString("-");
    musMinTxt.setCharacterSize(18);
    musMinTxt.setFillColor(sf::Color::White);
    musMinTxt.setPosition({ musMinR.position.x + 11.f, musMinR.position.y + 2.f });
    window.draw(musMinTxt);

    // Значение музыки
    sf::Text musVal(font);
    musVal.setString(std::to_string(settings.musicVolume) + "%");
    musVal.setCharacterSize(15);
    musVal.setFillColor(sf::Color(255, 225, 140));
    musVal.setPosition({ winX + 328.f, winY + 115.f });
    window.draw(musVal);

    // Кнопка [ + ]
    sf::FloatRect musPlsR = getSettingsMusicPlusRect();
    sf::RectangleShape musPls(musPlsR.size);
    musPls.setPosition(musPlsR.position);
    musPls.setFillColor(sf::Color(50, 36, 26));
    musPls.setOutlineColor(sf::Color(160, 120, 70));
    musPls.setOutlineThickness(1.5f);
    window.draw(musPls);

    sf::Text musPlsTxt(font);
    musPlsTxt.setString("+");
    musPlsTxt.setCharacterSize(18);
    musPlsTxt.setFillColor(sf::Color::White);
    musPlsTxt.setPosition({ musPlsR.position.x + 9.f, musPlsR.position.y + 2.f });
    window.draw(musPlsTxt);

    // 3. Блок управления
    sf::Text ctrlHeader(font);
    ctrlHeader.setString(toUtf8("УПРАВЛЕНИЕ:"));
    ctrlHeader.setCharacterSize(15);
    ctrlHeader.setFillColor(sf::Color(255, 215, 80));
    ctrlHeader.setStyle(sf::Text::Bold);
    ctrlHeader.setPosition({ winX + 30.f, winY + 165.f });
    window.draw(ctrlHeader);

    sf::RectangleShape ctrlBox({ winW - 60.f, 260.f });
    ctrlBox.setPosition({ winX + 30.f, winY + 195.f });
    ctrlBox.setFillColor(sf::Color(16, 14, 18, 220));
    ctrlBox.setOutlineColor(sf::Color(120, 95, 60));
    ctrlBox.setOutlineThickness(1.5f);
    window.draw(ctrlBox);

    const std::vector<std::pair<std::string, std::string>> controls = {
        { "WASD / Стрелки", "Перемещение персонажа" },
        { "E", "Взаимодействие / Вход в дом / Мебель" },
        { "I / Tab", "Инвентарь, журнал и крафт" },
        { "1 - 9", "Быстрый выбор слота хотбара" },
        { "Shift + ЛКМ", "Быстрый перенос в сундук и обратно" },
        { "ПКМ (зажатие)", "Подробный осмотр предмета и свойств" },
        { "R (у котла)", "Просмотр книги рецептов зелий" },
        { "ESC", "Меню паузы / Закрыть окна" }
    };

    float lineY = winY + 205.f;
    for (const auto& c : controls)
    {
        sf::Text keyTxt(font);
        keyTxt.setString(toUtf8(c.first));
        keyTxt.setCharacterSize(13);
        keyTxt.setFillColor(sf::Color(255, 220, 110));
        keyTxt.setPosition({ winX + 45.f, lineY });
        window.draw(keyTxt);

        sf::Text descTxt(font);
        descTxt.setString(toUtf8("—  " + c.second));
        descTxt.setCharacterSize(13);
        descTxt.setFillColor(sf::Color(215, 215, 215));
        descTxt.setPosition({ winX + 180.f, lineY });
        window.draw(descTxt);

        lineY += 29.f;
    }

    // Кнопка [ НАЗАД ]
    sf::FloatRect backR = getSettingsBackButtonRect();
    sf::RectangleShape backBtn(backR.size);
    backBtn.setPosition(backR.position);
    backBtn.setFillColor(sf::Color(65, 45, 30));
    backBtn.setOutlineColor(sf::Color(215, 170, 80));
    backBtn.setOutlineThickness(2.f);
    window.draw(backBtn);

    sf::Text backTxt(font);
    backTxt.setString(toUtf8("НАЗАД"));
    backTxt.setCharacterSize(16);
    backTxt.setFillColor(sf::Color(255, 255, 220));
    sf::FloatRect bb = backTxt.getLocalBounds();
    backTxt.setPosition({ backR.position.x + (backR.size.x - bb.size.x) / 2.f, backR.position.y + 10.f });
    window.draw(backTxt);
}

void UI::drawSavesModal(sf::RenderWindow& window,
    const std::vector<SaveSlotInfo>& saveSlots,
    bool isSaveMode,
    int pendingDeleteSlot)
{
    float winW = 580.f;
    float winH = 540.f;
    float winX = (static_cast<float>(window.getView().getSize().x) - winW) / 2.f;
    float winY = (static_cast<float>(window.getView().getSize().y) - winH) / 2.f;

    sf::Vector2i mpos = getMousePos(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    // Подложка модального окна
    sf::RectangleShape bg({ winW, winH });
    bg.setPosition({ winX, winY });
    bg.setFillColor(sf::Color(22, 19, 18, 252));
    bg.setOutlineColor(sf::Color(215, 170, 80));
    bg.setOutlineThickness(2.5f);
    window.draw(bg);

    // Заголовок
    sf::Text header(font);
    header.setString(toUtf8(isSaveMode ? "СОХРАНИТЬ ПРОГРЕСС" : "ЗАГРУЗИТЬ ИГРУ"));
    header.setCharacterSize(22);
    header.setFillColor(sf::Color(255, 220, 120));
    header.setStyle(sf::Text::Bold);
    sf::FloatRect hb = header.getLocalBounds();
    header.setPosition({ winX + (winW - hb.size.x) / 2.f, winY + 18.f });
    window.draw(header);

    // Подзаголовок
    sf::Text sub(font);
    sub.setString(toUtf8(isSaveMode ? "Выберите ячейку для сохранения данных вашей фермы"
                                    : "Выберите ячейку для продолжения игры"));
    sub.setCharacterSize(12);
    sub.setFillColor(sf::Color(170, 155, 135));
    sf::FloatRect sb = sub.getLocalBounds();
    sub.setPosition({ winX + (winW - sb.size.x) / 2.f, winY + 48.f });
    window.draw(sub);

    sf::RectangleShape line({ winW - 40.f, 1.5f });
    line.setPosition({ winX + 20.f, winY + 70.f });
    line.setFillColor(sf::Color(140, 105, 65));
    window.draw(line);

    // 3 ячейки сохранения
    for (int i = 0; i < 3; i++)
    {
        const SaveSlotInfo& sInfo = (i < static_cast<int>(saveSlots.size())) ? saveSlots[i] : SaveSlotInfo{};
        float slotY = winY + 82.f + i * 116.f;
        float slotW = winW - 40.f;
        float slotH = 98.f;
        float slotX = winX + 20.f;

        sf::RectangleShape card({ slotW, slotH });
        card.setPosition({ slotX, slotY });

        if (sInfo.exists)
        {
            card.setFillColor(sf::Color(35, 29, 23, 240));
            card.setOutlineColor(sf::Color(215, 170, 80));
            card.setOutlineThickness(2.f);
        }
        else
        {
            card.setFillColor(sf::Color(26, 23, 23, 200));
            card.setOutlineColor(sf::Color(90, 80, 75));
            card.setOutlineThickness(1.5f);
        }
        window.draw(card);

        // Текст слота
        sf::Text sTitle(font);
        if (sInfo.exists)
        {
            sTitle.setString(toUtf8("Слот " + std::to_string(i + 1) + ": " + sInfo.farmName + " (День " + std::to_string(sInfo.day) + ")"));
            sTitle.setFillColor(sf::Color(255, 230, 140));
            sTitle.setStyle(sf::Text::Bold);
        }
        else
        {
            sTitle.setString(toUtf8("Слот " + std::to_string(i + 1) + ": [ Пустая ячейка ]"));
            sTitle.setFillColor(sf::Color(145, 140, 135));
            sTitle.setStyle(sf::Text::Regular);
        }
        sTitle.setCharacterSize(15);
        sTitle.setPosition({ slotX + 16.f, slotY + 14.f });
        window.draw(sTitle);

        sf::Text sSub(font);
        if (sInfo.exists)
        {
            sSub.setString(toUtf8("Время: " + sInfo.timeStr + "  •  Золото: " + std::to_string(sInfo.gold) + " G\nСохранено: " + sInfo.timestamp));
            sSub.setFillColor(sf::Color(175, 215, 185));
        }
        else
        {
            sSub.setString(toUtf8("Свободно для записи игрового прогресса"));
            sSub.setFillColor(sf::Color(115, 110, 105));
        }
        sSub.setCharacterSize(12);
        sSub.setPosition({ slotX + 16.f, slotY + 40.f });
        window.draw(sSub);

        // Кнопки справа в карточке
        if (sInfo.exists)
        {
            // Главная кнопка действия (Сохранить или Загрузить)
            sf::FloatRect b1R = getSavesActionBtnRect(i);
            bool hov1 = b1R.contains({ mx, my });
            sf::RectangleShape b1(b1R.size);
            b1.setPosition(b1R.position);
            if (isSaveMode)
            {
                b1.setFillColor(hov1 ? sf::Color(160, 110, 45) : sf::Color(125, 80, 30));
                b1.setOutlineColor(sf::Color(255, 205, 100));
            }
            else
            {
                b1.setFillColor(hov1 ? sf::Color(55, 130, 55) : sf::Color(40, 95, 40));
                b1.setOutlineColor(sf::Color(140, 255, 140));
            }
            b1.setOutlineThickness(1.5f);
            window.draw(b1);

            sf::Text b1Txt(font);
            b1Txt.setString(toUtf8(isSaveMode ? "Записать" : "Загрузить"));
            b1Txt.setCharacterSize(14);
            b1Txt.setFillColor(sf::Color::White);
            sf::FloatRect b1b = b1Txt.getLocalBounds();
            b1Txt.setPosition({ b1R.position.x + (b1R.size.x - b1b.size.x) / 2.f, b1R.position.y + 8.f });
            window.draw(b1Txt);

            // Кнопка [Удалить]
            sf::FloatRect delR = getSavesDeleteBtnRect(i);
            bool isPending = (pendingDeleteSlot == (i + 1));
            bool hovDel = delR.contains({ mx, my });

            sf::RectangleShape delBtn(delR.size);
            delBtn.setPosition(delR.position);
            delBtn.setFillColor(isPending ? sf::Color(200, 40, 40) : (hovDel ? sf::Color(145, 40, 40) : sf::Color(100, 30, 30)));
            delBtn.setOutlineColor(sf::Color(240, 100, 100));
            delBtn.setOutlineThickness(isPending ? 2.f : 1.5f);
            window.draw(delBtn);

            sf::Text delTxt(font);
            delTxt.setString(toUtf8(isPending ? "Точно?" : "Удалить"));
            delTxt.setCharacterSize(13);
            delTxt.setFillColor(sf::Color(255, 235, 235));
            sf::FloatRect db = delTxt.getLocalBounds();
            delTxt.setPosition({ delR.position.x + (delR.size.x - db.size.x) / 2.f, delR.position.y + 9.f });
            window.draw(delTxt);
        }
        else
        {
            // Пустой слот
            if (isSaveMode)
            {
                sf::FloatRect bR = getSavesEmptyBtnRect(i);
                bool hov = bR.contains({ mx, my });
                sf::RectangleShape b(bR.size);
                b.setPosition(bR.position);
                b.setFillColor(hov ? sf::Color(55, 130, 55) : sf::Color(40, 95, 40));
                b.setOutlineColor(sf::Color(140, 255, 140));
                b.setOutlineThickness(1.5f);
                window.draw(b);

                sf::Text bTxt(font);
                bTxt.setString(toUtf8("Сохранить"));
                bTxt.setCharacterSize(14);
                bTxt.setFillColor(sf::Color::White);
                sf::FloatRect bb = bTxt.getLocalBounds();
                bTxt.setPosition({ bR.position.x + (bR.size.x - bb.size.x) / 2.f, bR.position.y + 8.f });
                window.draw(bTxt);
            }
            else
            {
                sf::Text emptyTxt(font);
                emptyTxt.setString(toUtf8("Пусто"));
                emptyTxt.setCharacterSize(14);
                emptyTxt.setFillColor(sf::Color(120, 115, 110));
                emptyTxt.setPosition({ slotX + slotW - 80.f, slotY + (slotH - 18.f) / 2.f });
                window.draw(emptyTxt);
            }
        }
    }

    // Кнопка [ НАЗАД ]
    sf::FloatRect backR = getSavesBackButtonRect();
    bool hovBack = backR.contains({ mx, my });
    sf::RectangleShape backBtn(backR.size);
    backBtn.setPosition(backR.position);
    backBtn.setFillColor(hovBack ? sf::Color(85, 60, 40) : sf::Color(65, 45, 30));
    backBtn.setOutlineColor(hovBack ? sf::Color(255, 215, 120) : sf::Color(215, 170, 80));
    backBtn.setOutlineThickness(2.f);
    window.draw(backBtn);

    sf::Text backTxt(font);
    backTxt.setString(toUtf8("НАЗАД"));
    backTxt.setCharacterSize(16);
    backTxt.setFillColor(sf::Color(255, 255, 220));
    sf::FloatRect bb = backTxt.getLocalBounds();
    backTxt.setPosition({ backR.position.x + (backR.size.x - bb.size.x) / 2.f, backR.position.y + 9.f });
    window.draw(backTxt);
}

void UI::drawAboutModal(sf::RenderWindow& window,
    int hoveredButton)
{
    float winX = 200.f;
    float winY = 120.f;
    float winW = 560.f;
    float winH = 530.f;

    sf::RectangleShape bg({ winW, winH });
    bg.setPosition({ winX, winY });
    bg.setFillColor(sf::Color(22, 19, 18, 250));
    bg.setOutlineColor(sf::Color(215, 170, 80));
    bg.setOutlineThickness(2.5f);
    window.draw(bg);

    sf::Text header(font);
    header.setString(toUtf8("ОБ ИГРЕ"));
    header.setCharacterSize(22);
    header.setFillColor(sf::Color(255, 220, 120));
    header.setStyle(sf::Text::Bold);
    sf::FloatRect hb = header.getLocalBounds();
    header.setPosition({ winX + (winW - hb.size.x) / 2.f, winY + 16.f });
    window.draw(header);

    sf::RectangleShape line({ winW - 40.f, 1.5f });
    line.setPosition({ winX + 20.f, winY + 48.f });
    line.setFillColor(sf::Color(140, 105, 65));
    window.draw(line);

    const std::vector<std::string> aboutLines = {
        "✦ Alchemy Farm ✦",
        "Уютный симулятор алхимической усадьбы и фермерства.",
        "Выращивайте волшебные растения, добывайте древесину и руду,",
        "обустраивайте свой дом и свободно расставляйте мебель.",
        "",
        "Особенности игры:",
        "• Алхимия: каменная ступка для порошков и варка эликсиров в котле.",
        "• Доска заказов: поручения жителей долины и деревенская лавка.",
        "• Ящик сбыта: ночная продажа предметов и утренний финансовый отчет.",
        "• Погода: динамический дождь, автоматически поливающий все грядки.",
        "• 3 слота сохранений: удобное сохранение и очистка прогресса.",
        "",
        "Управление: WASD - движение | 1-9 - хотбар | E - взаимодействие",
        "R - книга рецептов котла | ПКМ - осмотр | Shift+ЛКМ - быстрый перенос",
        "",
        "Версия: 1.0.0 Release | Разработчик: KotikHun | C++20 / SFML 3.0.2"
    };

    float lineY = winY + 58.f;
    for (const auto& l : aboutLines)
    {
        sf::Text t(font);
        t.setString(toUtf8(l));
        t.setCharacterSize(13);
        if (l.find("✦") != std::string::npos)
        {
            t.setCharacterSize(16);
            t.setFillColor(sf::Color(255, 220, 110));
            t.setStyle(sf::Text::Bold);
        }
        else if (l.find("Особенности игры:") != std::string::npos)
        {
            t.setCharacterSize(14);
            t.setFillColor(sf::Color(255, 215, 130));
            t.setStyle(sf::Text::Bold);
        }
        else if (l.find("Управление:") != std::string::npos)
        {
            t.setFillColor(sf::Color(160, 220, 255));
        }
        else if (l.find("Версия:") != std::string::npos)
        {
            t.setFillColor(sf::Color(180, 230, 200));
        }
        else
        {
            t.setFillColor(sf::Color(225, 220, 215));
        }
        sf::FloatRect tb = t.getLocalBounds();
        t.setPosition({ winX + (winW - tb.size.x) / 2.f, lineY });
        window.draw(t);
        lineY += (l.empty() ? 10.f : 23.f);
    }

    // Кнопка [ НАЗАД ]
    sf::FloatRect backR = getAboutBackButtonRect();
    sf::RectangleShape backBtn(backR.size);
    backBtn.setPosition(backR.position);
    backBtn.setFillColor(sf::Color(65, 45, 30));
    backBtn.setOutlineColor(sf::Color(215, 170, 80));
    backBtn.setOutlineThickness(2.f);
    if (hoveredButton == 0)
    {
        backBtn.setFillColor(sf::Color(95, 65, 40));
        backBtn.setOutlineColor(sf::Color(255, 220, 100));
    }
    window.draw(backBtn);

    sf::Text backTxt(font);
    backTxt.setString(toUtf8("НАЗАД"));
    backTxt.setCharacterSize(16);
    backTxt.setFillColor(sf::Color(255, 255, 220));
    sf::FloatRect bb = backTxt.getLocalBounds();
    backTxt.setPosition({ backR.position.x + (backR.size.x - bb.size.x) / 2.f, backR.position.y + 10.f });
    window.draw(backTxt);
}

// ============================================
// ЭКРАН НОЧНОГО ОТЧЁТА О ПРОДАЖАХ И СНЕ
// ============================================

sf::FloatRect UI::getSleepReportNextButtonRect()
{
    return sf::FloatRect({ 350.f, 600.f }, { 260.f, 48.f });
}

void UI::drawSleepReport(sf::RenderWindow& window,
    const SleepReport& report,
    float animTime,
    bool isNextHovered)
{
    float winW = static_cast<float>(window.getView().getSize().x);
    float winH = static_cast<float>(window.getView().getSize().y);

    // 1. Ночной градиентный фон со звёздами
    sf::VertexArray nightSky(sf::PrimitiveType::TriangleStrip, 4);
    nightSky[0].position = { 0.f, 0.f };
    nightSky[0].color = sf::Color(8, 10, 20);
    nightSky[1].position = { winW, 0.f };
    nightSky[1].color = sf::Color(14, 12, 28);
    nightSky[2].position = { 0.f, winH };
    nightSky[2].color = sf::Color(18, 16, 32);
    nightSky[3].position = { winW, winH };
    nightSky[3].color = sf::Color(26, 20, 36);
    window.draw(nightSky);

    // Мерцающие звёзды на фоне
    for (int i = 0; i < 30; i++)
    {
        float sx = std::fmod((i * 137.f + 53.f), winW);
        float sy = std::fmod((i * 93.f + 31.f), winH);
        float starAlpha = 100.f + 80.f * std::sin(animTime * 2.f + i * 1.5f);
        sf::CircleShape star(1.5f);
        star.setPosition({ sx, sy });
        star.setFillColor(sf::Color(230, 240, 255, static_cast<std::uint8_t>(starAlpha)));
        window.draw(star);
    }

    // Луна в верхнем правом углу
    sf::CircleShape moon(28.f);
    moon.setPosition({ winW - 90.f, 35.f });
    moon.setFillColor(sf::Color(255, 245, 200, 220));
    window.draw(moon);

    sf::CircleShape moonShadow(25.f);
    moonShadow.setPosition({ winW - 80.f, 31.f });
    moonShadow.setFillColor(sf::Color(10, 11, 22));
    window.draw(moonShadow);

    // 2. Центральная панель отчёта
    float panelW = 560.f;
    float panelH = 530.f;
    float panelX = (winW - panelW) / 2.f;
    float panelY = 125.f;

    sf::RectangleShape panelBg({ panelW, panelH });
    panelBg.setPosition({ panelX, panelY });
    panelBg.setFillColor(sf::Color(22, 19, 18, 250));
    panelBg.setOutlineColor(sf::Color(215, 170, 80));
    panelBg.setOutlineThickness(2.5f);
    window.draw(panelBg);

    sf::RectangleShape innerBorder({ panelW - 10.f, panelH - 10.f });
    innerBorder.setPosition({ panelX + 5.f, panelY + 5.f });
    innerBorder.setFillColor(sf::Color::Transparent);
    innerBorder.setOutlineColor(sf::Color(215, 170, 80, 80));
    innerBorder.setOutlineThickness(1.f);
    window.draw(innerBorder);

    // Заголовок
    sf::Text mainTitle(font);
    if (report.fullRest)
    {
        mainTitle.setString(toUtf8("СПОКОЙНОЙ НОЧИ..."));
        mainTitle.setFillColor(sf::Color(255, 225, 120));
    }
    else
    {
        mainTitle.setString(toUtf8("ПОТЕРЯ СОЗНАНИЯ В 02:00..."));
        mainTitle.setFillColor(sf::Color(255, 110, 110));
    }
    mainTitle.setCharacterSize(24);
    mainTitle.setStyle(sf::Text::Bold);
    sf::FloatRect mtb = mainTitle.getLocalBounds();
    mainTitle.setPosition({ panelX + (panelW - mtb.size.x) / 2.f, panelY + 18.f });
    window.draw(mainTitle);

    sf::Text subTitle(font);
    if (report.fullRest)
    {
        subTitle.setString(toUtf8("День " + std::to_string(report.dayNumber) + " завершён • Итоги отправки товаров"));
        subTitle.setFillColor(sf::Color(180, 225, 195));
    }
    else
    {
        subTitle.setString(toUtf8("Вас нашли без сил и доставили в кровать. Восстановлено 50% энергии."));
        subTitle.setFillColor(sf::Color(240, 190, 190));
    }
    subTitle.setCharacterSize(13);
    sf::FloatRect stb = subTitle.getLocalBounds();
    subTitle.setPosition({ panelX + (panelW - stb.size.x) / 2.f, panelY + 52.f });
    window.draw(subTitle);

    sf::RectangleShape divLine({ panelW - 40.f, 1.5f });
    divLine.setPosition({ panelX + 20.f, panelY + 76.f });
    divLine.setFillColor(sf::Color(140, 105, 65));
    window.draw(divLine);

    // 3. Список проданных товаров
    float listY = panelY + 90.f;
    float listH = 265.f;

    sf::RectangleShape listBg({ panelW - 40.f, listH });
    listBg.setPosition({ panelX + 20.f, listY });
    listBg.setFillColor(sf::Color(16, 14, 16, 200));
    listBg.setOutlineColor(sf::Color(100, 80, 55));
    listBg.setOutlineThickness(1.f);
    window.draw(listBg);

    if (report.soldItems.empty())
    {
        sf::Text emptyTxt(font);
        emptyTxt.setString(toUtf8("В ящик сбыта за сегодня ничего не отправлено.\nСкладывайте урожай, древесину, камни и зелья\nв деревянный ящик сбыта у дома перед сном!"));
        emptyTxt.setCharacterSize(14);
        emptyTxt.setFillColor(sf::Color(180, 175, 170));
        sf::FloatRect etb = emptyTxt.getLocalBounds();
        emptyTxt.setPosition({ panelX + (panelW - etb.size.x) / 2.f, listY + 85.f });
        window.draw(emptyTxt);

        sf::Text zeroEarned(font);
        zeroEarned.setString(toUtf8("Выручка: 0 G"));
        zeroEarned.setCharacterSize(15);
        zeroEarned.setFillColor(sf::Color(255, 215, 90));
        sf::FloatRect ztb = zeroEarned.getLocalBounds();
        zeroEarned.setPosition({ panelX + (panelW - ztb.size.x) / 2.f, listY + 165.f });
        window.draw(zeroEarned);
    }
    else
    {
        float itemRowY = listY + 10.f;
        int maxShown = std::min(6, static_cast<int>(report.soldItems.size()));

        for (int i = 0; i < maxShown; i++)
        {
            const auto& item = report.soldItems[i];

            // Иконка предмета
            drawItemIcon(window, panelX + 32.f, itemRowY - 2.f, Item{ item.id, item.count });

            // Название и количество
            sf::Text nameTxt(font);
            nameTxt.setString(toUtf8(item.name + "  x" + std::to_string(item.count)));
            nameTxt.setCharacterSize(14);
            nameTxt.setFillColor(sf::Color(255, 245, 225));
            nameTxt.setPosition({ panelX + 75.f, itemRowY + 6.f });
            window.draw(nameTxt);

            // Формула цены: "15g x 3 = "
            sf::Text formulaTxt(font);
            formulaTxt.setString(toUtf8(std::to_string(item.priceEach) + "g x " + std::to_string(item.count) + " ="));
            formulaTxt.setCharacterSize(13);
            formulaTxt.setFillColor(sf::Color(180, 180, 180));
            formulaTxt.setPosition({ panelX + 320.f, itemRowY + 7.f });
            window.draw(formulaTxt);

            // Сумма
            sf::Text totalTxt(font);
            totalTxt.setString("+" + std::to_string(item.totalPrice) + " G");
            totalTxt.setCharacterSize(15);
            totalTxt.setFillColor(sf::Color(140, 255, 140));
            totalTxt.setStyle(sf::Text::Bold);
            totalTxt.setPosition({ panelX + 430.f, itemRowY + 5.f });
            window.draw(totalTxt);

            itemRowY += 38.f;
        }

        if (report.soldItems.size() > 6)
        {
            sf::Text moreTxt(font);
            moreTxt.setString(toUtf8("... и ещё " + std::to_string(report.soldItems.size() - 6) + " позиций"));
            moreTxt.setCharacterSize(12);
            moreTxt.setFillColor(sf::Color(170, 170, 170));
            moreTxt.setPosition({ panelX + 75.f, itemRowY + 2.f });
            window.draw(moreTxt);
        }
    }

    // 4. Блок итогов и баланса
    sf::Text earnSum(font);
    earnSum.setString(toUtf8("Итого за ночь: +" + std::to_string(report.totalEarned) + " G"));
    earnSum.setCharacterSize(17);
    earnSum.setFillColor(sf::Color(255, 225, 90));
    earnSum.setStyle(sf::Text::Bold);
    earnSum.setPosition({ panelX + 35.f, panelY + listH + 105.f });
    window.draw(earnSum);

    sf::Text walletSum(font);
    walletSum.setString(toUtf8("Кошелек: " + std::to_string(report.currentGold) + " G"));
    walletSum.setCharacterSize(16);
    walletSum.setFillColor(sf::Color(255, 255, 235));
    sf::FloatRect wsb = walletSum.getLocalBounds();
    walletSum.setPosition({ panelX + panelW - wsb.size.x - 35.f, panelY + listH + 106.f });
    window.draw(walletSum);

    // 5. Кнопка "ПРОСНУТЬСЯ (ДАЛЕЕ)"
    sf::FloatRect nextRect = getSleepReportNextButtonRect();
    sf::RectangleShape nextBtn({ nextRect.size.x, nextRect.size.y });
    nextBtn.setPosition({ nextRect.position.x, nextRect.position.y });

    if (isNextHovered)
    {
        nextBtn.setFillColor(sf::Color(85, 60, 36));
        nextBtn.setOutlineColor(sf::Color(255, 225, 90));
        nextBtn.setOutlineThickness(2.5f);
    }
    else
    {
        nextBtn.setFillColor(sf::Color(45, 32, 22));
        nextBtn.setOutlineColor(sf::Color(190, 140, 70));
        nextBtn.setOutlineThickness(2.f);
    }
    window.draw(nextBtn);

    sf::Text nextTxt(font);
    std::string nStr = isNextHovered ? "✦  ПРОСНУТЬСЯ (ДАЛЕЕ)  ✦" : "ПРОСНУТЬСЯ (ДАЛЕЕ)";
    nextTxt.setString(toUtf8(nStr));
    nextTxt.setCharacterSize(16);
    nextTxt.setFillColor(isNextHovered ? sf::Color(255, 255, 230) : sf::Color(230, 215, 185));
    sf::FloatRect ntb = nextTxt.getLocalBounds();
    nextTxt.setPosition({ nextRect.position.x + (nextRect.size.x - ntb.size.x) / 2.f, nextRect.position.y + 12.f });
    window.draw(nextTxt);

    sf::Text keyHint(font);
    keyHint.setString(toUtf8("(Нажмите ПРОБЕЛ, ENTER или [E])"));
    keyHint.setCharacterSize(11);
    keyHint.setFillColor(sf::Color(160, 160, 180, 180));
    sf::FloatRect khb = keyHint.getLocalBounds();
    keyHint.setPosition({ (winW - khb.size.x) / 2.f, nextRect.position.y + nextRect.size.y + 8.f });
    window.draw(keyHint);
}

// ============================================
// ДОСКА ОБЪЯВЛЕНИЙ: ЗАКАЗЫ ЖИТЕЛЕЙ И ЛАВКА
// ============================================
sf::FloatRect UI::getOrdersBoardTabRect(int tabIndex)
{
    float winX = (960.f - 620.f) / 2.f;
    float winY = (800.f - 540.f) / 2.f;
    return sf::FloatRect({ winX + 30.f + tabIndex * 210.f, winY + 62.f }, { 200.f, 36.f });
}

sf::FloatRect UI::getOrdersBoardCloseButtonRect()
{
    float winX = (960.f - 620.f) / 2.f;
    float winY = (800.f - 540.f) / 2.f;
    return sf::FloatRect({ winX + 620.f - 40.f, winY + 16.f }, { 26.f, 26.f });
}

sf::FloatRect UI::getOrdersBoardQuestButtonRect(int questIndex)
{
    float winX = (960.f - 620.f) / 2.f;
    float winY = (800.f - 540.f) / 2.f;
    float startY = winY + 115.f + questIndex * 122.f;
    return sf::FloatRect({ winX + 620.f - 180.f, startY + 40.f }, { 150.f, 38.f });
}

sf::FloatRect UI::getOrdersBoardShopBuyButtonRect(int itemIndex)
{
    float winX = (960.f - 620.f) / 2.f;
    float winY = (800.f - 540.f) / 2.f;
    float startY = winY + 115.f + itemIndex * 72.f;
    return sf::FloatRect({ winX + 620.f - 145.f, startY + 16.f }, { 115.f, 34.f });
}

void UI::drawOrdersBoardModal(sf::RenderWindow& window,
    int activeTab,
    const std::vector<QuestRecord>& quests,
    const std::vector<ShopCatalogItem>& shopItems,
    const Inventory& inv,
    int playerGold,
    const ItemDatabase& itemDB,
    sf::Vector2f mousePos)
{
    float winW = 620.f;
    float winH = 540.f;
    float winX = (960.f - winW) / 2.f;
    float winY = (800.f - winH) / 2.f;

    float mx = mousePos.x;
    float my = mousePos.y;

    // Затемняющая подложка
    sf::RectangleShape dimRect({ 960.f, 800.f });
    dimRect.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(dimRect);

    // Главное окно
    sf::RectangleShape bg({ winW, winH });
    bg.setPosition({ winX, winY });
    bg.setFillColor(sf::Color(22, 19, 18, 252));
    bg.setOutlineColor(sf::Color(215, 170, 80));
    bg.setOutlineThickness(2.5f);
    window.draw(bg);

    // Заголовок
    sf::Text header(font);
    header.setString(toUtf8("ДОСКА ОБЪЯВЛЕНИЙ И ЗАКАЗОВ"));
    header.setCharacterSize(21);
    header.setFillColor(sf::Color(255, 220, 120));
    header.setStyle(sf::Text::Bold);
    sf::FloatRect hb = header.getLocalBounds();
    header.setPosition({ winX + 30.f, winY + 18.f });
    window.draw(header);

    // Золото игрока
    sf::Text goldTxt(font);
    goldTxt.setString(toUtf8("Золото: " + std::to_string(playerGold) + " g"));
    goldTxt.setCharacterSize(14);
    goldTxt.setFillColor(sf::Color(255, 230, 100));
    goldTxt.setStyle(sf::Text::Bold);
    goldTxt.setPosition({ winX + winW - 175.f, winY + 22.f });
    window.draw(goldTxt);

    // Кнопка [ X ] закрытия
    sf::FloatRect closeRect = getOrdersBoardCloseButtonRect();
    bool isCloseHov = closeRect.contains(mousePos);
    sf::RectangleShape closeBtn(closeRect.size);
    closeBtn.setPosition(closeRect.position);
    closeBtn.setFillColor(isCloseHov ? sf::Color(140, 40, 40) : sf::Color(65, 30, 30));
    closeBtn.setOutlineColor(sf::Color(200, 120, 120));
    closeBtn.setOutlineThickness(1.5f);
    window.draw(closeBtn);

    sf::Text closeTxt(font);
    closeTxt.setString(toUtf8("X"));
    closeTxt.setCharacterSize(14);
    closeTxt.setFillColor(sf::Color::White);
    closeTxt.setStyle(sf::Text::Bold);
    sf::FloatRect ctb = closeTxt.getLocalBounds();
    closeTxt.setPosition({ closeRect.position.x + (closeRect.size.x - ctb.size.x) / 2.f, closeRect.position.y + 4.f });
    window.draw(closeTxt);

    // Вкладки: 0 = "Заказы жителей", 1 = "Лавка припасов"
    for (int t = 0; t < 2; t++)
    {
        sf::FloatRect tr = getOrdersBoardTabRect(t);
        bool isTabHov = tr.contains(mousePos);
        bool isCurTab = (activeTab == t);

        sf::RectangleShape tabShape(tr.size);
        tabShape.setPosition(tr.position);
        if (isCurTab)
        {
            tabShape.setFillColor(sf::Color(55, 42, 30));
            tabShape.setOutlineColor(sf::Color(255, 215, 100));
            tabShape.setOutlineThickness(2.f);
        }
        else if (isTabHov)
        {
            tabShape.setFillColor(sf::Color(40, 32, 25));
            tabShape.setOutlineColor(sf::Color(180, 140, 75));
            tabShape.setOutlineThickness(1.5f);
        }
        else
        {
            tabShape.setFillColor(sf::Color(28, 22, 18));
            tabShape.setOutlineColor(sf::Color(110, 85, 55));
            tabShape.setOutlineThickness(1.f);
        }
        window.draw(tabShape);

        sf::Text tabTxt(font);
        tabTxt.setString(toUtf8(t == 0 ? "📋 Заказы жителей" : "🛒 Лавка припасов"));
        tabTxt.setCharacterSize(14);
        tabTxt.setFillColor(isCurTab ? sf::Color(255, 235, 160) : (isTabHov ? sf::Color(230, 210, 170) : sf::Color(170, 150, 125)));
        tabTxt.setStyle(isCurTab ? sf::Text::Bold : sf::Text::Regular);
        sf::FloatRect ttb = tabTxt.getLocalBounds();
        tabTxt.setPosition({ tr.position.x + (tr.size.x - ttb.size.x) / 2.f, tr.position.y + 8.f });
        window.draw(tabTxt);
    }

    sf::RectangleShape sepLine({ winW - 60.f, 1.5f });
    sepLine.setPosition({ winX + 30.f, winY + 104.f });
    sepLine.setFillColor(sf::Color(140, 105, 65));
    window.draw(sepLine);

    // ============================================
    // ВКЛАДКА 0: ЗАКАЗЫ ЖИТЕЛЕЙ
    // ============================================
    if (activeTab == 0)
    {
        for (size_t i = 0; i < quests.size() && i < 3; i++)
        {
            const auto& q = quests[i];
            float qy = winY + 115.f + static_cast<float>(i) * 122.f;

            // Карточка заказа
            sf::RectangleShape card({ winW - 60.f, 112.f });
            card.setPosition({ winX + 30.f, qy });
            card.setFillColor(q.isCompleted ? sf::Color(24, 32, 24, 230) : sf::Color(32, 26, 22, 230));
            card.setOutlineColor(q.isCompleted ? sf::Color(90, 160, 90) : sf::Color(130, 100, 65));
            card.setOutlineThickness(1.5f);
            window.draw(card);

            // Имя заказчика (бэйдж)
            sf::Text clientTxt(font);
            clientTxt.setString(toUtf8("От: " + q.clientName));
            clientTxt.setCharacterSize(14);
            clientTxt.setFillColor(sf::Color(255, 210, 100));
            clientTxt.setStyle(sf::Text::Bold);
            clientTxt.setPosition({ winX + 45.f, qy + 10.f });
            window.draw(clientTxt);

            // Название / Описание задачи
            sf::Text descTxt(font);
            descTxt.setString(toUtf8(wrapString(font, q.description, 13, 380.f)));
            descTxt.setCharacterSize(13);
            descTxt.setFillColor(sf::Color(230, 225, 215));
            descTxt.setPosition({ winX + 45.f, qy + 31.f });
            window.draw(descTxt);

            // Подсказка / Лор (автоперенос, чтобы текст никогда не заходил под кнопку)
            if (!q.hint.empty())
            {
                sf::Text hintTxt(font);
                hintTxt.setString(toUtf8(wrapString(font, q.hint, 11, 380.f)));
                hintTxt.setCharacterSize(11);
                hintTxt.setFillColor(sf::Color(150, 175, 160));
                hintTxt.setStyle(sf::Text::Italic);
                hintTxt.setPosition({ winX + 45.f, qy + 50.f });
                window.draw(hintTxt);
            }

            // Наличие в инвентаре
            int inInv = inv.countItem(q.targetItemId);
            bool hasEnough = (inInv >= q.targetCount);

            sf::Text invTxt(font);
            std::string invStr = "В наличии: " + std::to_string(inInv) + " / " + std::to_string(q.targetCount);
            invTxt.setString(toUtf8(invStr));
            invTxt.setCharacterSize(12);
            invTxt.setFillColor(hasEnough ? sf::Color(120, 240, 120) : sf::Color(230, 120, 100));
            invTxt.setStyle(sf::Text::Bold);
            invTxt.setPosition({ winX + 45.f, qy + 84.f });
            window.draw(invTxt);

            // Награда
            sf::Text rewardTxt(font);
            rewardTxt.setString(toUtf8("Награда: +" + std::to_string(q.rewardGold) + " g"));
            rewardTxt.setCharacterSize(13);
            rewardTxt.setFillColor(sf::Color(255, 225, 80));
            rewardTxt.setStyle(sf::Text::Bold);
            rewardTxt.setPosition({ winX + 620.f - 180.f, qy + 12.f });
            window.draw(rewardTxt);

            // Кнопка сдачи заказа
            sf::FloatRect btnRect = getOrdersBoardQuestButtonRect(static_cast<int>(i));
            bool isBtnHov = btnRect.contains(mousePos);

            sf::RectangleShape qBtn(btnRect.size);
            qBtn.setPosition(btnRect.position);

            sf::Text btnTxt(font);
            btnTxt.setCharacterSize(13);
            btnTxt.setStyle(sf::Text::Bold);

            if (q.isCompleted)
            {
                qBtn.setFillColor(sf::Color(35, 65, 35));
                qBtn.setOutlineColor(sf::Color(80, 150, 80));
                qBtn.setOutlineThickness(1.5f);
                btnTxt.setString(toUtf8("✔ ВЫПОЛНЕНО"));
                btnTxt.setFillColor(sf::Color(160, 230, 160));
            }
            else if (hasEnough)
            {
                qBtn.setFillColor(isBtnHov ? sf::Color(90, 70, 30) : sf::Color(65, 50, 22));
                qBtn.setOutlineColor(isBtnHov ? sf::Color(255, 230, 90) : sf::Color(210, 160, 60));
                qBtn.setOutlineThickness(2.f);
                btnTxt.setString(toUtf8("СДАТЬ ЗАКАЗ"));
                btnTxt.setFillColor(isBtnHov ? sf::Color(255, 255, 230) : sf::Color(240, 220, 160));
            }
            else
            {
                qBtn.setFillColor(sf::Color(35, 30, 28));
                qBtn.setOutlineColor(sf::Color(75, 65, 60));
                qBtn.setOutlineThickness(1.f);
                btnTxt.setString(toUtf8("НЕ ХВАТАЕТ"));
                btnTxt.setFillColor(sf::Color(140, 130, 125));
            }
            window.draw(qBtn);

            sf::FloatRect btb = btnTxt.getLocalBounds();
            btnTxt.setPosition({ btnRect.position.x + (btnRect.size.x - btb.size.x) / 2.f, btnRect.position.y + 10.f });
            window.draw(btnTxt);
        }
    }
    // ============================================
    // ВКЛАДКА 1: ЛАВКА ПРИПАСОВ
    // ============================================
    else
    {
        for (size_t i = 0; i < shopItems.size() && i < 5; i++)
        {
            const auto& item = shopItems[i];
            float iy = winY + 115.f + static_cast<float>(i) * 74.f;

            // Карточка товара
            sf::RectangleShape card({ winW - 60.f, 66.f });
            card.setPosition({ winX + 30.f, iy });
            card.setFillColor(sf::Color(32, 26, 22, 230));
            card.setOutlineColor(sf::Color(120, 95, 60));
            card.setOutlineThickness(1.5f);
            window.draw(card);

            // Название предмета
            sf::Text nameTxt(font);
            nameTxt.setString(toUtf8(item.name));
            nameTxt.setCharacterSize(14);
            nameTxt.setFillColor(sf::Color(255, 225, 140));
            nameTxt.setStyle(sf::Text::Bold);
            nameTxt.setPosition({ winX + 45.f, iy + 10.f });
            window.draw(nameTxt);

            // Описание
            sf::Text descTxt(font);
            descTxt.setString(toUtf8(item.desc));
            descTxt.setCharacterSize(12);
            descTxt.setFillColor(sf::Color(180, 170, 160));
            descTxt.setPosition({ winX + 45.f, iy + 34.f });
            window.draw(descTxt);

            // Цена
            sf::Text priceTxt(font);
            priceTxt.setString(toUtf8("Цена: " + std::to_string(item.price) + " g"));
            priceTxt.setCharacterSize(13);
            priceTxt.setFillColor(sf::Color(255, 215, 80));
            priceTxt.setStyle(sf::Text::Bold);
            priceTxt.setPosition({ winX + 620.f - 250.f, iy + 22.f });
            window.draw(priceTxt);

            // Кнопка покупки
            sf::FloatRect buyRect = getOrdersBoardShopBuyButtonRect(static_cast<int>(i));
            bool isBuyHov = buyRect.contains(mousePos);
            bool canAfford = (playerGold >= item.price);

            sf::RectangleShape buyBtn(buyRect.size);
            buyBtn.setPosition(buyRect.position);

            sf::Text bTxt(font);
            bTxt.setString(toUtf8("КУПИТЬ"));
            bTxt.setCharacterSize(13);
            bTxt.setStyle(sf::Text::Bold);

            if (canAfford)
            {
                buyBtn.setFillColor(isBuyHov ? sf::Color(90, 70, 30) : sf::Color(65, 50, 22));
                buyBtn.setOutlineColor(isBuyHov ? sf::Color(255, 230, 90) : sf::Color(210, 160, 60));
                buyBtn.setOutlineThickness(2.f);
                bTxt.setFillColor(isBuyHov ? sf::Color(255, 255, 230) : sf::Color(240, 220, 160));
            }
            else
            {
                buyBtn.setFillColor(sf::Color(35, 30, 28));
                buyBtn.setOutlineColor(sf::Color(75, 65, 60));
                buyBtn.setOutlineThickness(1.f);
                bTxt.setFillColor(sf::Color(140, 130, 125));
            }
            window.draw(buyBtn);

            sf::FloatRect btb = bTxt.getLocalBounds();
            bTxt.setPosition({ buyRect.position.x + (buyRect.size.x - btb.size.x) / 2.f, buyRect.position.y + 8.f });
            window.draw(bTxt);
        }
    }
}
