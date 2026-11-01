#include "SleepReportUI.hpp"
#include "UI.hpp"
#include <cmath>
#include <algorithm>

sf::FloatRect SleepReportUI::getNextButtonRect()
{
    return sf::FloatRect({ 350.f, 600.f }, { 260.f, 48.f });
}

void SleepReportUI::draw(sf::RenderWindow& window,
                         const sf::Font& font,
                         const UI& ui,
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
            ui.drawItemIcon(window, panelX + 32.f, itemRowY - 2.f, Item{ item.id, item.count });

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
    sf::FloatRect nextRect = getNextButtonRect();
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
