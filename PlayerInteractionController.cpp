#include "PlayerInteractionController.hpp"
#include "DayNightCycle.hpp"
#include <algorithm>

bool PlayerInteractionController::handleUseAction(Player& player,
                                                 Inventory& inv,
                                                 int activeSlot,
                                                 World& world,
                                                 House& house,
                                                 std::vector<Chest>& chests,
                                                 std::vector<Furniture>& furniture,
                                                 Chest* nearbyChest,
                                                 int nearbyFurnitureIdx,
                                                 AlchemySystem& alchemy,
                                                 PlacementSystem& placement,
                                                 SoundManager& sound,
                                                 const ItemDatabase& itemDB,
                                                 std::vector<FloatingText>& floatingTexts,
                                                 std::vector<DroppedItem>& droppedItems,
                                                 bool& isTransitioningSleep,
                                                 bool isTransitioningWake,
                                                 float& fadeTarget,
                                                 SleepReport& sleepReport)
{
    sf::Vector2f ppos = player.shape.getPosition();
    const Item& activeItem = inv.getSlot(activeSlot);
    bool hasTool = (activeItem.id == "axe" || activeItem.id == "pickaxe");

    // 1. Двери (вход и выход из дома по направлению взгляда)
    if (house.isInside && house.isNearExitDoor(ppos) && player.dirY > 0)
    {
        house.exit(player);
        return true;
    }
    if (!house.isInside && house.isNearExteriorDoor(ppos) && player.dirY < 0)
    {
        house.enter(player);
        return true;
    }

    // 2. Если в руках топор или кирка: попытка забрать мебель
    if (hasTool)
    {
        if (placement.tryPickupFurniture(house, player, inv, chests, furniture, floatingTexts, itemDB, nearbyChest, nearbyFurnitureIdx))
        {
            player.playAction(activeItem.id == "axe" ? PlayerAction::Axe : PlayerAction::Pickaxe);
            return true;
        }
    }

    // 3. Сундук
    if (nearbyChest != nullptr)
    {
        nearbyChest->isOpen = !nearbyChest->isOpen;
        return true;
    }

    // 4. Мебель (кровать, котел, стол)
    if (nearbyFurnitureIdx >= 0 && nearbyFurnitureIdx < static_cast<int>(furniture.size()))
    {
        auto& f = furniture[nearbyFurnitureIdx];
        if (f.id == "bed")
        {
            DayNightCycle::sleepInBed(true, isTransitioningSleep, isTransitioningWake, fadeTarget, sleepReport, sound);
            return true;
        }
        if (f.id == "cauldron")
        {
            alchemy.isCauldronOpen = !alchemy.isCauldronOpen;
            return true;
        }
        if (f.id == "alchemist_table")
        {
            alchemy.isTableOpen = !alchemy.isTableOpen;
            return true;
        }
    }

    // 5. Установка мебели из рук перед собой (сундук, кровать, котел, стол, ковер, окно)
    if (activeItem.id == "chest" || activeItem.id == "bed" || activeItem.id == "alchemist_table" ||
        activeItem.id == "cauldron" || activeItem.id == "carpet" || activeItem.id == "window")
    {
        if (placement.tryPlaceFurniture(activeItem.id, house, world, player, inv, chests, furniture, floatingTexts, itemDB))
        {
            return true;
        }
    }

    // 6. Набор воды во флакон
    if (activeItem.id == "empty_flask")
    {
        int pcx = static_cast<int>((ppos.x + Player::SIZE / 2.f) / World::TILE_SIZE) + player.dirX;
        int pcy = static_cast<int>((ppos.y + Player::SIZE / 2.f) / World::TILE_SIZE) + player.dirY;
        if (pcx >= 0 && pcx < World::WIDTH && pcy >= 0 && pcy < World::HEIGHT)
        {
            if (world.map[pcy][pcx].type == 2) // Вода
            {
                inv.removeItem("empty_flask", 1);
                inv.addItem("water_flask", 1, 99);
                sound.playWater();
                FloatingText ft;
                ft.text = "+1 Флакон с чистой водой";
                ft.position = { ppos.x - 20.f, ppos.y - 25.f };
                ft.color = sf::Color(140, 220, 255);
                ft.timer = 1.4f;
                floatingTexts.push_back(ft);
                return true;
            }
        }
    }

    // 7. Анимация действий инструментами
    if (activeItem.id == "hoe") { player.playAction(PlayerAction::Hoe); player.useEnergy(2.f); sound.playHoe(); }
    else if (activeItem.id == "watering_can") { player.playAction(PlayerAction::Water); player.useEnergy(1.f); sound.playWater(); }
    else if (activeItem.id == "axe") { player.playAction(PlayerAction::Axe); player.useEnergy(2.f); sound.playAxe(); }
    else if (activeItem.id == "pickaxe") { player.playAction(PlayerAction::Pickaxe); player.useEnergy(2.f); sound.playPickaxe(); }

    // Взаимодействие с миром (грядки, сорняки, деревья)
    if (!house.isInside)
    {
        world.interact(player, activeItem.id, inv, droppedItems);
    }
    return true;
}

bool PlayerInteractionController::handleDropItem(Player& player,
                                                 Inventory& inv,
                                                 int activeSlot,
                                                 std::vector<DroppedItem>& droppedItems)
{
    const Item& itm = inv.getSlot(activeSlot);
    if (!itm.isEmpty())
    {
        DroppedItem drop;
        drop.item = Item{ itm.id, 1 };
        drop.position = {
            player.shape.getPosition().x + player.dirX * 24.f,
            player.shape.getPosition().y + player.dirY * 24.f
        };
        droppedItems.push_back(drop);
        inv.removeItem(itm.id, 1);
        return true;
    }
    return false;
}

bool PlayerInteractionController::handleConsumeOrUseItem(Player& player,
                                                         Inventory& inv,
                                                         int activeSlot,
                                                         World& world,
                                                         House& house,
                                                         std::vector<Chest>& chests,
                                                         std::vector<Furniture>& furniture,
                                                         PlacementSystem& placement,
                                                         const ItemDatabase& itemDB,
                                                         std::vector<FloatingText>& floatingTexts)
{
    const Item& itm = inv.getSlot(activeSlot);
    if (itm.isEmpty()) return false;

    sf::Vector2f ppos = player.shape.getPosition();
    if (itm.id == "energy_potion")
    {
        inv.removeItem("energy_potion", 1);
        inv.addItem("empty_flask", 1, 99);
        player.energy = std::min(player.maxEnergy, player.energy + 50.f);
        player.isExhausted = false;

        FloatingText ft;
        ft.text = "+50 Энергии!";
        ft.position = ppos;
        ft.color = sf::Color(120, 255, 140);
        ft.timer = 1.5f;
        floatingTexts.push_back(ft);
        return true;
    }
    else if (itm.id == "speed_potion")
    {
        inv.removeItem("speed_potion", 1);
        inv.addItem("empty_flask", 1, 99);
        player.speedBuffTimer = 30.f;

        FloatingText ft;
        ft.text = "+40% Скорости (30 сек)!";
        ft.position = ppos;
        ft.color = sf::Color(255, 230, 90);
        ft.timer = 1.5f;
        floatingTexts.push_back(ft);
        return true;
    }
    else if (itm.id == "growth_potion")
    {
        inv.removeItem("growth_potion", 1);
        inv.addItem("empty_flask", 1, 99);

        // Ускоряем грядки вокруг игрока
        int pcx = static_cast<int>((ppos.x + Player::SIZE / 2.f) / World::TILE_SIZE);
        int pcy = static_cast<int>((ppos.y + Player::SIZE / 2.f) / World::TILE_SIZE);
        for (int dy = -2; dy <= 2; dy++)
        {
            for (int dx = -2; dx <= 2; dx++)
            {
                int tx = pcx + dx;
                int ty = pcy + dy;
                if (tx >= 0 && tx < World::WIDTH && ty >= 0 && ty < World::HEIGHT)
                {
                    if (world.map[ty][tx].plantStage > 0 && world.map[ty][tx].plantStage < 4)
                    {
                        world.map[ty][tx].plantStage = 4; // Мгновенный урожай
                    }
                }
            }
        }

        FloatingText ft;
        ft.text = "Эликсир роста: Посевы созрели!";
        ft.position = ppos;
        ft.color = sf::Color(230, 160, 255);
        ft.timer = 1.8f;
        floatingTexts.push_back(ft);
        return true;
    }
    else if (itm.id == "berry")
    {
        inv.removeItem("berry", 1);
        player.energy = std::min(player.maxEnergy, player.energy + 15.f);
        player.isExhausted = false;

        FloatingText ft;
        ft.text = "+15 Энергии";
        ft.position = ppos;
        ft.color = sf::Color(255, 180, 200);
        ft.timer = 1.2f;
        floatingTexts.push_back(ft);
        return true;
    }
    else if (itm.id == "chest" || itm.id == "bed" || itm.id == "alchemist_table" ||
             itm.id == "cauldron" || itm.id == "carpet" || itm.id == "window")
    {
        return placement.tryPlaceFurniture(itm.id, house, world, player, inv, chests, furniture, floatingTexts, itemDB);
    }

    return false;
}
