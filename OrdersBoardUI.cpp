#include "OrdersBoardUI.hpp"
#include "UI.hpp"
#include "UIUtils.hpp"
#include "Inventory.hpp"
#include "ItemDatabase.hpp"

sf::FloatRect OrdersBoardUI::getTabRect(int tabIndex)
{
    float winX = (960.f - 620.f) / 2.f;
    float winY = (800.f - 540.f) / 2.f;
    return sf::FloatRect({ winX + 30.f + tabIndex * 210.f, winY + 62.f }, { 200.f, 36.f });
}

sf::FloatRect OrdersBoardUI::getCloseButtonRect()
{
    float winX = (960.f - 620.f) / 2.f;
    float winY = (800.f - 540.f) / 2.f;
    return sf::FloatRect({ winX + 620.f - 40.f, winY + 16.f }, { 26.f, 26.f });
}

sf::FloatRect OrdersBoardUI::getQuestButtonRect(int questIndex)
{
    float winX = (960.f - 620.f) / 2.f;
    float winY = (800.f - 540.f) / 2.f;
    float startY = winY + 115.f + questIndex * 122.f;
    return sf::FloatRect({ winX + 620.f - 180.f, startY + 40.f }, { 150.f, 38.f });
}

sf::FloatRect OrdersBoardUI::getShopBuyButtonRect(int itemIndex)
{
    float winX = (960.f - 620.f) / 2.f;
    float winY = (800.f - 540.f) / 2.f;
    float startY = winY + 115.f + itemIndex * 72.f;
    return sf::FloatRect({ winX + 620.f - 145.f, startY + 16.f }, { 115.f, 34.f });
}

void OrdersBoardUI::draw(sf::RenderWindow& window,
                         const sf::Font& font,
                         const UI& ui,
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
    sf::FloatRect closeRect = getCloseButtonRect();
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
        sf::FloatRect tr = getTabRect(t);
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

            // Подсказка / Лор
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
            sf::FloatRect btnRect = getQuestButtonRect(static_cast<int>(i));
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
            sf::FloatRect buyRect = getShopBuyButtonRect(static_cast<int>(i));
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
