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
        q1.hint = "Подсказка: Соберите созревшие ягоды на грядках фермы.";
        dailyQuests.push_back(q1);

        QuestRecord q2;
        q2.id = "q_chamomile";
        q2.clientName = "Лекарь Элиас";
        q2.title = "Аптечная ромашка";
        q2.description = "Требуется: 3 Ромашки для успокаивающего чая";
        q2.targetItemId = "chamomile";
        q2.targetCount = 3;
        q2.rewardGold = 85;
        q2.hint = "Подсказка: Посадите семена ромашки на политую грядку.";
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
        q2.id = "q_chamomile_powder";
        q2.clientName = "Травница Элизабет";
        q2.title = "Ромашковая пыльца";
        q2.description = "Требуется: 2 Ромашковой пыльцы для мазей";
        q2.targetItemId = "chamomile_powder";
        q2.targetCount = 2;
        q2.rewardGold = 140;
        q2.hint = "Подсказка: Разотрите цветки ромашки в алхимической ступке.";
        dailyQuests.push_back(q2);

        QuestRecord q3;
        q3.id = "q_decoction";
        q3.clientName = "Стражник Гарет";
        q3.title = "Ромашковый отвар";
        q3.description = "Требуется: 1 Ромашковый отвар перед ночным дозором";
        q3.targetItemId = "chamomile_decoction";
        q3.targetCount = 1;
        q3.rewardGold = 95;
        q3.hint = "Подсказка: Сварите в котле 1 воду + 2 ромашки.";
        dailyQuests.push_back(q3);
    }
    else
    {
        QuestRecord q1;
        q1.id = "q_berry_juice";
        q1.clientName = "Старейшина Гораций";
        q1.title = "Ягодный морс";
        q1.description = "Требуется: 1 Освежающий ягодный морс";
        q1.targetItemId = "berry_juice";
        q1.targetCount = 1;
        q1.rewardGold = 85;
        q1.hint = "Подсказка: Сварите в котле воду + 2 ягоды (или 1 порошок).";
        dailyQuests.push_back(q1);

        QuestRecord q2;
        q2.id = "q_chamomile_tea";
        q2.clientName = "Ведьма Селена";
        q2.title = "Густой ромашковый чай";
        q2.description = "Требуется: 1 Густой ромашковый чай для ритуала сна";
        q2.targetItemId = "chamomile_tea";
        q2.targetCount = 1;
        q2.rewardGold = 175;
        q2.hint = "Подсказка: Сварите в котле воду + 4 ромашки (или 2 пыльцы).";
        dailyQuests.push_back(q2);

        QuestRecord q3;
        q3.id = "q_stone";
        q3.clientName = "Каменщик Дарин";
        q3.title = "Прочный камень";
        q3.description = "Требуется: 4 Камня для укрепления фундамента";
        q3.targetItemId = "stone";
        q3.targetCount = 4;
        q3.rewardGold = 65;
        q3.hint = "Подсказка: Раскалывайте серые булыжники киркой.";
        dailyQuests.push_back(q3);
    }
}

void QuestManager::initShopCatalog()
{
    shopCatalog.clear();

    ShopCatalogItem i1;
    i1.itemId = "chamomile_seeds";
    i1.name = "Семена ромашки";
    i1.price = 20;
    i1.desc = "Аптечная ромашка для отваров и чая.";
    shopCatalog.push_back(i1);

    ShopCatalogItem i2;
    i2.itemId = "seeds";
    i2.name = "Пакетик семян ягод";
    i2.price = 15;
    i2.desc = "Быстрорастущие сочные лесные ягоды.";
    shopCatalog.push_back(i2);

    ShopCatalogItem i3;
    i3.itemId = "empty_flask";
    i3.name = "Пустой флакон";
    i3.price = 20;
    i3.desc = "Стеклянная колба для набора воды и зелий.";
    shopCatalog.push_back(i3);

    ShopCatalogItem i4;
    i4.itemId = "chamomile_decoction";
    i4.name = "Ромашковый отвар";
    i4.price = 60;
    i4.desc = "Защищает от бессонной ночи (+20 сил).";
    shopCatalog.push_back(i4);

    ShopCatalogItem i5;
    i5.itemId = "wood";
    i5.name = "Связка бревен (3 шт)";
    i5.price = 45;
    i5.desc = "Плотная дубовая древесина для крафта.";
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
