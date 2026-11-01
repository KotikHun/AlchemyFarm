#include "QuestManager.hpp"
#include "UI.hpp"
#include "Inventory.hpp"
#include "Player.hpp"
#include "SoundManager.hpp"
#include "FloatingText.hpp"

QuestManager::QuestManager()
{
    initDailyQuests(1);
    initShopCatalog();
}

void QuestManager::initDailyQuests(int day)
{
    dailyQuests.clear();

    if (day % 3 == 1)
    {
        QuestRecord q1;
        q1.id = "q_berries";
        q1.clientName = "Травница Элизабет";
        q1.title = "Сбор диких ягод";
        q1.description = "Требуется: 3 Ягоды для целебного отвара";
        q1.targetItemId = "berry";
        q1.targetCount = 3;
        q1.rewardGold = 70;
        q1.hint = "Подсказка: Разотрите ягоды в ступке алхимика в порошок.";
        dailyQuests.push_back(q1);

        QuestRecord q2;
        q2.id = "q_water";
        q2.clientName = "Кузнец Брок";
        q2.title = "Родниковая вода";
        q2.description = "Требуется: 1 Флакон с водой для закалки стали";
        q2.targetItemId = "water_flask";
        q2.targetCount = 1;
        q2.rewardGold = 45;
        q2.hint = "Подсказка: Возьмите флакон в руки и нажмите [E] у пруда.";
        dailyQuests.push_back(q2);

        QuestRecord q3;
        q3.id = "q_wood";
        q3.clientName = "Лесник Робин";
        q3.title = "Дубовые бревна";
        q3.description = "Требуется: 5 Древесины для починки забора";
        q3.targetItemId = "wood";
        q3.targetCount = 5;
        q3.rewardGold = 80;
        q3.hint = "Подсказка: Срубите дерево топором, ударив по стволу.";
        dailyQuests.push_back(q3);
    }
    else if (day % 3 == 2)
    {
        QuestRecord q1;
        q1.id = "q_powder";
        q1.clientName = "Алхимик Мортимер";
        q1.title = "Ягодный порошок";
        q1.description = "Требуется: 2 Порошка из ягод для проведения опыта";
        q1.targetItemId = "berry_powder";
        q1.targetCount = 2;
        q1.rewardGold = 130;
        q1.hint = "Подсказка: Положите ягоды в ступку на столе и перетрите.";
        dailyQuests.push_back(q1);

        QuestRecord q2;
        q2.id = "q_stone";
        q2.clientName = "Каменщик Дарин";
        q2.title = "Прочный камень";
        q2.description = "Требуется: 4 Камня для укрепления фундамента";
        q2.targetItemId = "stone";
        q2.targetCount = 4;
        q2.rewardGold = 65;
        q2.hint = "Подсказка: Раскалывайте серые булыжники киркой.";
        dailyQuests.push_back(q2);

        QuestRecord q3;
        q3.id = "q_energy";
        q3.clientName = "Стражник Гарет";
        q3.title = "Зелье бодрости";
        q3.description = "Требуется: 1 Зелье энергии перед ночным дозором";
        q3.targetItemId = "energy_potion";
        q3.targetCount = 1;
        q3.rewardGold = 170;
        q3.hint = "Подсказка: Сварите в котле воду + ягодный порошок.";
        dailyQuests.push_back(q3);
    }
    else
    {
        QuestRecord q1;
        q1.id = "q_herbal";
        q1.clientName = "Ведьма Селена";
        q1.title = "Травяная пудра";
        q1.description = "Требуется: 2 Растительного порошка для зельеварения";
        q1.targetItemId = "herbal_powder";
        q1.targetCount = 2;
        q1.rewardGold = 150;
        q1.hint = "Подсказка: Измельчите листву дерева или сорняки в ступке.";
        dailyQuests.push_back(q1);

        QuestRecord q2;
        q2.id = "q_growth";
        q2.clientName = "Садовник Оливер";
        q2.title = "Зелье стимуляции роста";
        q2.description = "Требуется: 1 Зелье роста для молодых саженцев";
        q2.targetItemId = "growth_potion";
        q2.targetCount = 1;
        q2.rewardGold = 190;
        q2.hint = "Подсказка: Сварите воду + растительный порошок в котле.";
        dailyQuests.push_back(q2);

        QuestRecord q3;
        q3.id = "q_fiber";
        q3.clientName = "Ткачиха Марта";
        q3.title = "Крепкие волокна";
        q3.description = "Требуется: 3 Волокна для плетения веревок и сумок";
        q3.targetItemId = "fiber";
        q3.targetCount = 3;
        q3.rewardGold = 55;
        q3.hint = "Подсказка: Срезайте дикие травы косой на полях фермы.";
        dailyQuests.push_back(q3);
    }
}

void QuestManager::initShopCatalog()
{
    shopCatalog.clear();

    ShopCatalogItem i1;
    i1.itemId = "seeds";
    i1.name = "Пакетик семян";
    i1.price = 15;
    i1.desc = "Быстрорастущие ягоды для фермы.";
    shopCatalog.push_back(i1);

    ShopCatalogItem i2;
    i2.itemId = "empty_flask";
    i2.name = "Пустой флакон";
    i2.price = 20;
    i2.desc = "Стеклянная колба для набора воды и зелий.";
    shopCatalog.push_back(i2);

    ShopCatalogItem i3;
    i3.itemId = "wood";
    i3.name = "Связка бревен (3 шт)";
    i3.price = 45;
    i3.desc = "Плотная дубовая древесина для крафта.";
    shopCatalog.push_back(i3);

    ShopCatalogItem i4;
    i4.itemId = "stone";
    i4.name = "Каменные блоки (3 шт)";
    i4.price = 40;
    i4.desc = "Строительный гранит для мебели и очага.";
    shopCatalog.push_back(i4);

    ShopCatalogItem i5;
    i5.itemId = "energy_potion";
    i5.name = "Эликсир бодрости";
    i5.price = 90;
    i5.desc = "Мгновенно восстанавливает 50 энергии.";
    shopCatalog.push_back(i5);
}

void QuestManager::handleEvents(const sf::Event& event,
                                sf::Vector2f mpos,
                                Inventory& inv,
                                Player& player,
                                SoundManager& sound,
                                std::vector<FloatingText>& floatingTexts)
{
    if (const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseBtn->button == sf::Mouse::Button::Left)
        {
            // 1. Кнопка закрытия [X]
            if (UI::getOrdersBoardCloseButtonRect().contains(mpos))
            {
                sound.playClick();
                isOpen = false;
                return;
            }

            // 2. Переключение вкладок
            for (int t = 0; t < 2; t++)
            {
                if (UI::getOrdersBoardTabRect(t).contains(mpos))
                {
                    sound.playClick();
                    activeTab = t;
                    return;
                }
            }

            // 3. Вкладка заказов: сдача квестов
            if (activeTab == 0)
            {
                for (size_t i = 0; i < dailyQuests.size() && i < 3; i++)
                {
                    auto& q = dailyQuests[i];
                    if (!q.isCompleted && UI::getOrdersBoardQuestButtonRect(static_cast<int>(i)).contains(mpos))
                    {
                        if (inv.countItem(q.targetItemId) >= q.targetCount)
                        {
                            inv.removeItem(q.targetItemId, q.targetCount);
                            player.gold += q.rewardGold;
                            q.isCompleted = true;
                            sound.playCoin();

                            FloatingText ft;
                            ft.text = "Заказ сдан: +" + std::to_string(q.rewardGold) + " золота!";
                            ft.position = { player.shape.getPosition().x - 60.f, player.shape.getPosition().y - 30.f };
                            ft.color = sf::Color(255, 230, 90);
                            ft.timer = 2.5f;
                            floatingTexts.push_back(ft);
                        }
                        return;
                    }
                }
            }
            // 4. Вкладка лавки: покупка товаров
            else if (activeTab == 1)
            {
                for (size_t i = 0; i < shopCatalog.size() && i < 5; i++)
                {
                    const auto& item = shopCatalog[i];
                    if (UI::getOrdersBoardShopBuyButtonRect(static_cast<int>(i)).contains(mpos))
                    {
                        if (player.gold >= item.price)
                        {
                            int count = (item.itemId == "wood" || item.itemId == "stone") ? 3 : 1;
                            player.gold -= item.price;
                            inv.addItem(item.itemId, count, 99);
                            sound.playCoin();

                            FloatingText ft;
                            ft.text = "Куплено: " + item.name + " (-" + std::to_string(item.price) + "g)";
                            ft.position = { player.shape.getPosition().x - 60.f, player.shape.getPosition().y - 30.f };
                            ft.color = sf::Color(140, 255, 140);
                            ft.timer = 2.0f;
                            floatingTexts.push_back(ft);
                        }
                        return;
                    }
                }
            }
        }
    }
    else if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape || key->code == sf::Keyboard::Key::E)
        {
            isOpen = false;
        }
    }
}
