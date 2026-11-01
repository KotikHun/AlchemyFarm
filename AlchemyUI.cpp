#include "AlchemyUI.hpp"
#include "UI.hpp"
#include "UIUtils.hpp"
#include "Inventory.hpp"
#include "AlchemySystem.hpp"
#include "ItemDatabase.hpp"
#include <cmath>
#include <algorithm>

void AlchemyUI::drawInteractiveCauldron(sf::RenderWindow& window,
    const sf::Font& font,
    UI& ui,
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
                if (d) ui.hoveredItemName = d->name + " (Кликните, чтобы положить в котел)";
            }

            sf::RectangleShape s({ slotSize, slotSize });
            s.setPosition({ sx, sy });
            s.setFillColor(hov ? sf::Color(60, 65, 80) : sf::Color(38, 38, 45));
            s.setOutlineColor(hov ? sf::Color(140, 200, 255) : sf::Color(70, 70, 80));
            s.setOutlineThickness(1.f);
            window.draw(s);

            if (!item.isEmpty())
            {
                ui.drawItemIcon(window, sx + (slotSize - 32.f) / 2.f - 9.f, sy + (slotSize - 32.f) / 2.f - 9.f, item);
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
            if (d) ui.hoveredItemName = d->name + " (Кликните, чтобы забрать)";
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
            ui.drawItemIcon(window, sx + 4.f - 9.f, sy + 6.f - 9.f, ingredients[i]);
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
        if (base.isEmpty()) ui.hoveredItemName = "Колба основы (поместите флакон с чистой водой)";
        else {
            const ItemData* d = itemDB.find(base.id);
            if (d) ui.hoveredItemName = d->name + " (Основа варки)";
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
        ui.drawItemIcon(window, baseSlotX + 4.f - 9.f, baseSlotY + 4.f - 9.f, base);
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
        drawCauldronRecipeBook(window, font, ui, winX, winY, alchemy, itemDB);
    }
}

// ============================================
// ВСПЛЫВАЮЩЕЕ ОКНО РЕЦЕПТОВ В КОТЛЕ
// ============================================
void AlchemyUI::drawCauldronRecipeBook(sf::RenderWindow& window, const sf::Font& font, UI& ui, float winX, float winY, const AlchemySystem& alchemy, const ItemDatabase& itemDB)
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
            ui.drawItemIcon(window, bx + 24.f, rowY + 16.f, Item{ r.resultId, 1 });

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
void AlchemyUI::drawMortarStation(sf::RenderWindow& window,
    const sf::Font& font,
    UI& ui,
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
                if (d) ui.hoveredItemName = d->name + " (Кликните для ступки)";
            }

            sf::RectangleShape s({ slotSize, slotSize });
            s.setPosition({ sx, sy });
            s.setFillColor(hov ? sf::Color(60, 65, 80) : sf::Color(38, 38, 45));
            s.setOutlineColor(hov ? sf::Color(140, 200, 255) : sf::Color(70, 70, 80));
            s.setOutlineThickness(1.f);
            window.draw(s);

            if (!item.isEmpty())
            {
                ui.drawItemIcon(window, sx + (slotSize - 32.f) / 2.f - 9.f, sy + (slotSize - 32.f) / 2.f - 9.f, item);
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
        ui.drawItemIcon(window, inX - 9.f, inY - 9.f, alchemy.mortarInput);
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
        ui.drawItemIcon(window, outX - 9.f, outY - 9.f, alchemy.mortarOutput);
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

