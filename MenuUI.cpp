#include "MenuUI.hpp"
#include "UIUtils.hpp"
#include "SaveManager.hpp"
#include <cmath>
#include <algorithm>

sf::FloatRect MenuUI::getMainMenuButtonRect(int index)
{
    float btnW = 260.f;
    float btnH = 50.f;
    float btnX = 350.f;
    float startY = 270.f;
    float spacing = 65.f;
    return sf::FloatRect({ btnX, startY + index * spacing }, { btnW, btnH });
}

sf::FloatRect MenuUI::getPauseMenuButtonRect(int index)
{
    float btnW = 250.f;
    float btnH = 46.f;
    float btnX = 355.f;
    float startY = 238.f;
    float spacing = 58.f;
    return sf::FloatRect({ btnX, startY + index * spacing }, { btnW, btnH });
}

sf::FloatRect MenuUI::getAboutBackButtonRect()
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

sf::FloatRect MenuUI::getSettingsBackButtonRect()
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

sf::FloatRect MenuUI::getSettingsSoundMinusRect()
{
    float winX = 220.f;
    float winY = 120.f;
    return sf::FloatRect({ winX + 280.f, winY + 66.f }, { 32.f, 28.f });
}

sf::FloatRect MenuUI::getSettingsSoundPlusRect()
{
    float winX = 220.f;
    float winY = 120.f;
    return sf::FloatRect({ winX + 385.f, winY + 66.f }, { 32.f, 28.f });
}

sf::FloatRect MenuUI::getSettingsMusicMinusRect()
{
    float winX = 220.f;
    float winY = 120.f;
    return sf::FloatRect({ winX + 280.f, winY + 111.f }, { 32.f, 28.f });
}

sf::FloatRect MenuUI::getSettingsMusicPlusRect()
{
    float winX = 220.f;
    float winY = 120.f;
    return sf::FloatRect({ winX + 385.f, winY + 111.f }, { 32.f, 28.f });
}

sf::FloatRect MenuUI::getSavesBackButtonRect()
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

sf::FloatRect MenuUI::getSavesActionBtnRect(int slotIndex)
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

sf::FloatRect MenuUI::getSavesDeleteBtnRect(int slotIndex)
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

sf::FloatRect MenuUI::getSavesEmptyBtnRect(int slotIndex)
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

void MenuUI::drawMainMenu(sf::RenderWindow& window,
    const sf::Font& font,
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
            drawSettingsModal(window, font, settings, hoveredButton);
        }
        else if (modal == MenuModal::Saves)
        {
            drawSavesModal(window, font, saveSlots, isSaveMode, pendingDeleteSlot);
        }
        else if (modal == MenuModal::About)
        {
            drawAboutModal(window, font, hoveredButton);
        }
    }
}

void MenuUI::drawLoadingScreen(sf::RenderWindow& window,
    const sf::Font& font,
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

void MenuUI::drawPauseMenu(sf::RenderWindow& window,
    const sf::Font& font,
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
            drawSettingsModal(window, font, settings, hoveredButton);
        }
        else if (modal == MenuModal::Saves)
        {
            drawSavesModal(window, font, saveSlots, isSaveMode, pendingDeleteSlot);
        }
    }
}

void MenuUI::drawSettingsModal(sf::RenderWindow& window,
    const sf::Font& font,
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

void MenuUI::drawSavesModal(sf::RenderWindow& window,
    const sf::Font& font,
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

void MenuUI::drawAboutModal(sf::RenderWindow& window,
    const sf::Font& font,
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

