#include "UI.hpp"
#include "Player.hpp"
#include "AlchemySystem.hpp"
#include "SaveManager.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

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
    loadIcon("chamomile", "assets/sprites/icons/chamomile.png");
    loadIcon("chamomile_seeds", "assets/sprites/icons/chamomile_seeds.png");
    loadIcon("chamomile_powder", "assets/sprites/icons/chamomile_powder.png");
    loadIcon("chamomile_decoction", "assets/sprites/icons/chamomile_decoction.png");
    loadIcon("chamomile_tea", "assets/sprites/icons/chamomile_tea.png");
    loadIcon("chamomile_tincture", "assets/sprites/icons/chamomile_tincture.png");
    loadIcon("berry_juice", "assets/sprites/icons/berry_juice.png");
    loadIcon("berry_decoction", "assets/sprites/icons/berry_decoction.png");
    loadIcon("berry_elixir", "assets/sprites/icons/berry_elixir.png");
    loadIcon("forest_infusion", "assets/sprites/icons/forest_infusion.png");
    loadIcon("tonic_infusion", "assets/sprites/icons/tonic_infusion.png");
    loadIcon("grand_nectar", "assets/sprites/icons/grand_nectar.png");
    loadIcon("acorn", "assets/sprites/icons/acorn.png");

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
    const AlchemySystem* alchemy,
    const std::vector<QuestRecord>* dailyQuests)
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
        drawJournalTab(window, journalCategory, alchemy, dailyQuests, &inv);
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
    const Item& item) const
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
void UI::drawJournalTab(sf::RenderWindow& window, int selectedCategory, const AlchemySystem* alchemy, const std::vector<QuestRecord>* dailyQuests, const Inventory* inv)
{
    float startX = 235.f;
    float startY = 220.f;
    float pageW = 350.f;
    float pageH = 270.f;

    sf::Vector2i mpos = getMousePos(window);
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    // 1. Вкладки категорий СПРАВА (закладки книги: Задания, Зелья, Растения, Ремесло)
    const char* categoryNames[] = { "Задания", "Зелья", "Растения", "Ремесло" };
    float tabX = startX + pageW + 10.f; // 595.f
    float tabW = 120.f;
    float tabH = 30.f;

    for (int c = 0; c < 4; c++)
    {
        float ty = startY + 12.f + c * 38.f;
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
        catTxt.setCharacterSize(13);
        catTxt.setFillColor(isSel ? sf::Color(255, 230, 130) : sf::Color(175, 175, 185));
        sf::FloatRect cb = catTxt.getLocalBounds();
        catTxt.setPosition({ tabX + (tabW - cb.size.x) / 2.f, ty + 6.f });
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
    header.setCharacterSize(15);
    header.setFillColor(sf::Color(255, 220, 110));
    header.setPosition({ startX + 12.f, startY + 10.f });

    if (selectedCategory == 0) // Задания (Ежедневные заказы)
    {
        header.setString(toUtf8("Ежедневные заказы жителей"));
        window.draw(header);

        if (dailyQuests != nullptr && !dailyQuests->empty())
        {
            float rowY = startY + 34.f;
            for (size_t i = 0; i < dailyQuests->size() && i < 3; i++)
            {
                const auto& q = (*dailyQuests)[i];
                sf::RectangleShape rowBox({ pageW - 20.f, 68.f });
                rowBox.setPosition({ startX + 10.f, rowY });
                rowBox.setFillColor(q.isCompleted ? sf::Color(30, 45, 34, 220) : sf::Color(32, 28, 38, 200));
                rowBox.setOutlineColor(q.isCompleted ? sf::Color(100, 200, 120) : sf::Color(120, 95, 70));
                rowBox.setOutlineThickness(1.f);
                window.draw(rowBox);

                drawItemIcon(window, startX + 14.f, rowY + 14.f, Item{ q.targetItemId, q.targetCount });

                // Клиент и название
                sf::Text qTitle(font);
                qTitle.setString(toUtf8(q.clientName + ": " + q.title));
                qTitle.setCharacterSize(12);
                qTitle.setFillColor(sf::Color(255, 225, 120));
                qTitle.setPosition({ startX + 54.f, rowY + 6.f });
                window.draw(qTitle);

                // Описание задачи
                sf::Text qDesc(font);
                qDesc.setString(toUtf8(q.description));
                qDesc.setCharacterSize(11);
                qDesc.setFillColor(sf::Color(210, 215, 230));
                qDesc.setPosition({ startX + 54.f, rowY + 25.f });
                window.draw(qDesc);

                // Статус
                sf::Text qStatus(font);
                if (q.isCompleted)
                {
                    qStatus.setString(toUtf8("[Сдано] Награда получена"));
                    qStatus.setFillColor(sf::Color(130, 255, 150));
                }
                else
                {
                    int hasCount = inv ? inv->countItem(q.targetItemId) : 0;
                    if (hasCount >= q.targetCount)
                    {
                        qStatus.setString(toUtf8("[Готово] Сдайте на доске! (+" + std::to_string(q.rewardGold) + "g)"));
                        qStatus.setFillColor(sf::Color(255, 230, 90));
                    }
                    else
                    {
                        qStatus.setString(toUtf8("В наличии: " + std::to_string(hasCount) + "/" + std::to_string(q.targetCount) + " (Награда: " + std::to_string(q.rewardGold) + "g)"));
                        qStatus.setFillColor(sf::Color(180, 180, 195));
                    }
                }
                qStatus.setCharacterSize(10);
                qStatus.setPosition({ startX + 54.f, rowY + 46.f });
                window.draw(qStatus);

                rowY += 74.f;
            }
        }
        else
        {
            sf::Text noQ(font);
            noQ.setString(toUtf8("Нет активных поручений на сегодня.\nПроверьте доску заказов завтра!"));
            noQ.setCharacterSize(12);
            noQ.setFillColor(sf::Color(180, 180, 190));
            noQ.setPosition({ startX + 20.f, startY + 60.f });
            window.draw(noQ);
        }
    }
    else if (selectedCategory == 1) // Зелья
    {
        header.setString(toUtf8("Книга рецептов зельеварения"));
        window.draw(header);

        if (alchemy != nullptr)
        {
            const auto& rlist = alchemy->getRecipes();
            float rowY = startY + 34.f;
            int countShown = 0;
            for (size_t i = 0; i < rlist.size() && countShown < 3; i++)
            {
                const auto& r = rlist[i];
                if (!r.discovered && countShown >= 2) continue; // показать первые открытые или одну закрытую

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
                    rEff.setCharacterSize(10);
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
                    rHint.setString(toUtf8("Экспериментируйте у котла (клавиша R)\nс чистой водой, травами и порошками!"));
                    rHint.setCharacterSize(11);
                    rHint.setFillColor(sf::Color(130, 130, 145));
                    rHint.setPosition({ startX + 18.f, rowY + 30.f });
                    window.draw(rHint);
                }

                rowY += 74.f;
                countShown++;
            }
        }
    }
    else if (selectedCategory == 2) // Растения
    {
        header.setString(toUtf8("Атлас трав, культур и погоды"));
        window.draw(header);

        sf::Text body(font);
        body.setString(toUtf8(
            "- Аптечная ромашка:\n"
            "  Выращивается из семян. Основа для отваров и чая.\n"
            "  Защищает от ночного обморока и восстанавливает силы!\n\n"
            "- Лесная ягода:\n"
            "  Растет на полях фермы. Дает мгновенный заряд бодрости\n"
            "  или перетирается в ступке в алхимический порошок.\n\n"
            "- Ромашковая пыльца и порошки (1 порошок = 2 сырья):\n"
            "  Максимальная насыщенность зелий достигается только\n"
            "  через тонкий помол в ступке алхимического стола!\n\n"
            "- Полив и погода:\n"
            "  Поливайте грядки лейкой каждый день. В дождь\n"
            "  все вспаханные грядки поливаются автоматически!"
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
            "  золото и покупайте семена ромашки в лавке.\n\n"
            "- Ящик сбыта товаров (Shipping Bin):\n"
            "  Складывайте урожай и зелья — ночью они\n"
            "  автоматически продаются с подробным отчетом!\n\n"
            "- Котел и ступка:\n"
            "  Измельчайте сырье и варите чудодейственные зелья."
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


// ============================================
// ДЕЛЕГИРОВАНИЕ В ВЫДЕЛЕННЫЕ UI МОДУЛИ
// ============================================

void UI::drawInteractiveCauldron(sf::RenderWindow& window,
    const Inventory& inv,
    const AlchemySystem& alchemy,
    const ItemDatabase& itemDB)
{
    AlchemyUI::drawInteractiveCauldron(window, font, *this, inv, alchemy, itemDB);
}

void UI::drawCauldronRecipeBook(sf::RenderWindow& window, float winX, float winY, const AlchemySystem& alchemy, const ItemDatabase& itemDB)
{
    AlchemyUI::drawCauldronRecipeBook(window, font, *this, winX, winY, alchemy, itemDB);
}

void UI::drawMortarStation(sf::RenderWindow& window,
    const Inventory& inv,
    const AlchemySystem& alchemy,
    const ItemDatabase& itemDB)
{
    AlchemyUI::drawMortarStation(window, font, *this, inv, alchemy, itemDB);
}

// Меню и модалки
sf::FloatRect UI::getMainMenuButtonRect(int index) { return MenuUI::getMainMenuButtonRect(index); }
sf::FloatRect UI::getPauseMenuButtonRect(int index) { return MenuUI::getPauseMenuButtonRect(index); }
sf::FloatRect UI::getAboutBackButtonRect() { return MenuUI::getAboutBackButtonRect(); }
sf::FloatRect UI::getSettingsBackButtonRect() { return MenuUI::getSettingsBackButtonRect(); }
sf::FloatRect UI::getSettingsSoundMinusRect() { return MenuUI::getSettingsSoundMinusRect(); }
sf::FloatRect UI::getSettingsSoundPlusRect() { return MenuUI::getSettingsSoundPlusRect(); }
sf::FloatRect UI::getSettingsMusicMinusRect() { return MenuUI::getSettingsMusicMinusRect(); }
sf::FloatRect UI::getSettingsMusicPlusRect() { return MenuUI::getSettingsMusicPlusRect(); }
sf::FloatRect UI::getSavesBackButtonRect() { return MenuUI::getSavesBackButtonRect(); }
sf::FloatRect UI::getSavesActionBtnRect(int slotIndex) { return MenuUI::getSavesActionBtnRect(slotIndex); }
sf::FloatRect UI::getSavesDeleteBtnRect(int slotIndex) { return MenuUI::getSavesDeleteBtnRect(slotIndex); }
sf::FloatRect UI::getSavesEmptyBtnRect(int slotIndex) { return MenuUI::getSavesEmptyBtnRect(slotIndex); }

void UI::drawMainMenu(sf::RenderWindow& window, float animTime, int hoveredButton, MenuModal modal, const GameSettings& settings, const std::vector<SaveSlotInfo>& saveSlots, bool isSaveMode, int pendingDeleteSlot, const std::vector<MenuParticle>& particles)
{
    MenuUI::drawMainMenu(window, font, animTime, hoveredButton, modal, settings, saveSlots, isSaveMode, pendingDeleteSlot, particles);
}

void UI::drawLoadingScreen(sf::RenderWindow& window, float progress, const std::string& stageText, const std::string& tipText, float animTime)
{
    MenuUI::drawLoadingScreen(window, font, progress, stageText, tipText, animTime);
}

void UI::drawPauseMenu(sf::RenderWindow& window, int hoveredButton, MenuModal modal, const GameSettings& settings, const std::vector<SaveSlotInfo>& saveSlots, bool isSaveMode, int pendingDeleteSlot)
{
    MenuUI::drawPauseMenu(window, font, hoveredButton, modal, settings, saveSlots, isSaveMode, pendingDeleteSlot);
}

void UI::drawSettingsModal(sf::RenderWindow& window, const GameSettings& settings, int hoveredButton)
{
    MenuUI::drawSettingsModal(window, font, settings, hoveredButton);
}

void UI::drawSavesModal(sf::RenderWindow& window, const std::vector<SaveSlotInfo>& saveSlots, bool isSaveMode, int pendingDeleteSlot)
{
    MenuUI::drawSavesModal(window, font, saveSlots, isSaveMode, pendingDeleteSlot);
}

void UI::drawAboutModal(sf::RenderWindow& window, int hoveredButton)
{
    MenuUI::drawAboutModal(window, font, hoveredButton);
}

// Отчёт о сне
sf::FloatRect UI::getSleepReportNextButtonRect() { return SleepReportUI::getNextButtonRect(); }
void UI::drawSleepReport(sf::RenderWindow& window, const SleepReport& report, float animTime, bool isNextHovered)
{
    SleepReportUI::draw(window, font, *this, report, animTime, isNextHovered);
}

// Доска объявлений и лавка
sf::FloatRect UI::getOrdersBoardTabRect(int tabIndex) { return OrdersBoardUI::getTabRect(tabIndex); }
sf::FloatRect UI::getOrdersBoardCloseButtonRect() { return OrdersBoardUI::getCloseButtonRect(); }
sf::FloatRect UI::getOrdersBoardQuestButtonRect(int questIndex) { return OrdersBoardUI::getQuestButtonRect(questIndex); }
sf::FloatRect UI::getOrdersBoardShopBuyButtonRect(int itemIndex) { return OrdersBoardUI::getShopBuyButtonRect(itemIndex); }

void UI::drawOrdersBoardModal(sf::RenderWindow& window, int activeTab, const std::vector<QuestRecord>& quests, const std::vector<ShopCatalogItem>& shopItems, const Inventory& inv, int playerGold, const ItemDatabase& itemDB, sf::Vector2f mousePos)
{
    OrdersBoardUI::draw(window, font, *this, activeTab, quests, shopItems, inv, playerGold, itemDB, mousePos);
}
