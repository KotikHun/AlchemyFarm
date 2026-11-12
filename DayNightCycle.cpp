#include "DayNightCycle.hpp"
#include <map>
#include <string>

void DayNightCycle::sleepInBed(bool fullRest,
                               bool& isTransitioningSleep,
                               bool isTransitioningWake,
                               float& fadeTarget,
                               SleepReport& sleepReport,
                               SoundManager& sound)
{
    if (isTransitioningSleep || isTransitioningWake) return;
    isTransitioningSleep = true;
    fadeTarget = 255.f;
    sleepReport.fullRest = fullRest;
    sound.playSleep();
}

void DayNightCycle::triggerSleep(bool fullRest,
                                 SleepReport& sleepReport,
                                 TimeSystem& timeSystem,
                                 Player& player,
                                 std::vector<Chest>& chests,
                                 const ItemDatabase& itemDB,
                                 bool& isInventoryOpen,
                                 AlchemySystem& alchemy,
                                 Chest* nearbyChest,
                                 bool& isSleepNextHovered)
{
    sleepReport.fullRest = fullRest;
    sleepReport.dayNumber = timeSystem.currentDay;
    sleepReport.soldItems.clear();
    sleepReport.totalEarned = 0;
    sleepReport.previousGold = player.gold;

    // Ищем ящик сбыта (Shipping Bin)
    Chest* shippingBin = nullptr;
    for (auto& c : chests)
    {
        if (c.type == "shipping_bin")
        {
            shippingBin = &c;
            break;
        }
    }

    if (shippingBin != nullptr)
    {
        std::map<std::string, int> aggregated;
        for (int i = 0; i < shippingBin->slotCount; i++)
        {
            const Item& itm = shippingBin->inventory.getSlot(i);
            if (!itm.isEmpty())
            {
                aggregated[itm.id] += itm.count;
            }
        }

        for (const auto& [id, count] : aggregated)
        {
            const ItemData* d = itemDB.find(id);
            int priceEach = d ? d->sellPrice : 0;
            int total = priceEach * count;

            SoldItemRecord rec;
            rec.id = id;
            rec.name = d ? d->name : id;
            rec.count = count;
            rec.priceEach = priceEach;
            rec.totalPrice = total;
            sleepReport.soldItems.push_back(rec);
            sleepReport.totalEarned += total;
        }

        // Очищаем ящик сбыта после ночной продажи
        shippingBin->inventory = Inventory();
        shippingBin->isOpen = false;
    }

    player.gold += sleepReport.totalEarned;
    sleepReport.currentGold = player.gold;

    // Закрываем любые открытые окна
    isInventoryOpen = false;
    alchemy.isCauldronOpen = false;
    alchemy.isTableOpen = false;
    alchemy.showRecipes = false;
    if (nearbyChest) nearbyChest->isOpen = false;

    isSleepNextHovered = false;
}

void DayNightCycle::wakeUpFromSleep(SleepReport& sleepReport,
                                    TimeSystem& timeSystem,
                                    World& world,
                                    House& house,
                                    Player& player,
                                    QuestManager& quests,
                                    Inventory& inv,
                                    std::vector<Chest>& chests,
                                    std::vector<Furniture>& furniture,
                                    AlchemySystem& alchemy,
                                    std::vector<FloatingText>& floatingTexts,
                                    int currentSaveSlot,
                                    std::vector<SaveSlotInfo>& cachedSaveSlots)
{
    timeSystem.advanceDay(sleepReport.fullRest);
    world.updateGrowth();

    // Автоматический полив грядок в дождливый день
    if (timeSystem.isRaining)
    {
        for (int y = 0; y < World::HEIGHT; y++)
        {
            for (int x = 0; x < World::WIDTH; x++)
            {
                if (world.map[y][x].type == 1) // Вспаханная грядка
                {
                    world.map[y][x].watered = true;
                }
            }
        }
    }

    // Обновление ежедневных заказов жителей
    quests.initDailyQuests(timeSystem.currentDay);

    house.isInside = true;
    player.shape.setPosition({ House::ROOM_X + 32.f + 16.f, House::ROOM_Y + 85.f + 10.f });

    if (sleepReport.fullRest || player.chamomileProtection)
    {
        player.energy = player.maxEnergy;
        player.isExhausted = false;

        FloatingText ft;
        if (!sleepReport.fullRest && player.chamomileProtection)
        {
            ft.text = "Ромашковый отвар спас вас! Новый день (энергия 100%).";
            ft.color = sf::Color(160, 255, 180);
        }
        else
        {
            ft.text = "Доброе утро! Новый день наступил (энергия 100%).";
            ft.color = sf::Color(140, 255, 160);
        }
        ft.position = { player.shape.getPosition().x - 120.f, player.shape.getPosition().y - 40.f };
        ft.timer = 2.5f;
        floatingTexts.push_back(ft);

        player.chamomileProtection = false; // расходуем эффект
    }
    else
    {
        player.energy = player.maxEnergy * 0.5f;
        player.isExhausted = false;

        FloatingText ft;
        ft.text = "Вы очнулись в кровати после обморока (энергия 50%).";
        ft.position = { player.shape.getPosition().x - 130.f, player.shape.getPosition().y - 40.f };
        ft.color = sf::Color(255, 110, 110);
        ft.timer = 3.0f;
        floatingTexts.push_back(ft);
    }

    if (timeSystem.isRaining)
    {
        FloatingText rft;
        rft.text = "Сегодня идет дождь! Все грядки политы.";
        rft.position = { player.shape.getPosition().x - 90.f, player.shape.getPosition().y - 15.f };
        rft.color = sf::Color(140, 220, 255);
        rft.timer = 3.0f;
        floatingTexts.push_back(rft);
    }

    // Автоматическое сохранение прогресса в активный слот при пробуждении
    SaveManager::saveGame(currentSaveSlot, player, inv, timeSystem, world, house, chests, furniture, alchemy);
    cachedSaveSlots = SaveManager::getAllSlotsInfo();
}
