#include "PlacementSystem.hpp"
#include "World.hpp"
#include "House.hpp"
#include "Player.hpp"
#include "Inventory.hpp"
#include "ItemDatabase.hpp"
#include "FloatingText.hpp"
#include "DroppedItem.hpp"
#include <cmath>
#include <cstdlib>

PlacementSystem::PlacementSystem()
{
    loadTextures();
}

void PlacementSystem::loadTextures()
{
    if (chestTexture.loadFromFile("assets/sprites/chest.png"))
    {
        chestSprite = std::make_unique<sf::Sprite>(chestTexture);
        chestSprite->setTextureRect(sf::IntRect({ 0, 0 }, { 16, 16 }));
        chestSprite->setScale({ 2.f, 2.f });
    }

    if (bedTexture.loadFromFile("assets/sprites/bed_wooden.png"))
    {
        bedSprite = std::make_unique<sf::Sprite>(bedTexture);
        bedSprite->setOrigin({ 16.f, 55.f });
        bedSprite->setScale({ 1.5f, 1.5f });
    }

    if (tableTexture.loadFromFile("assets/sprites/alchemy_table_clean.png") ||
        tableTexture.loadFromFile("assets/sprites/alchemy_table_anim.png"))
    {
        tableSprite = std::make_unique<sf::Sprite>(tableTexture);
    }

    if (cauldronAnimTexture.loadFromFile("assets/sprites/cauldron_anim.png"))
    {
        cauldronAnimSprite = std::make_unique<sf::Sprite>(cauldronAnimTexture);
    }

    if (carpetTexture.loadFromFile("assets/sprites/carpet.png"))
    {
        carpetSprite = std::make_unique<sf::Sprite>(carpetTexture);
    }
}

void PlacementSystem::drawFurniturePiece(sf::RenderWindow& window, const Furniture& f)
{
    if (f.id == "bed")
    {
        if (bedSprite)
        {
            bedSprite->setPosition(f.position);
            window.draw(*bedSprite);
        }
    }
    else if (f.id == "carpet")
    {
        if (carpetSprite)
        {
            carpetSprite->setPosition(f.position);
            window.draw(*carpetSprite);
        }
    }
    else if (f.id == "alchemist_table")
    {
        if (tableSprite)
        {
            tableSprite->setTextureRect(sf::IntRect({ 0, 0 }, { 48, 64 }));
            tableSprite->setPosition(f.position);
            window.draw(*tableSprite);
        }
    }
    else if (f.id == "cauldron")
    {
        if (cauldronAnimSprite)
        {
            cauldronAnimSprite->setTextureRect(sf::IntRect({ f.animFrame * 36, 0 }, { 36, 40 }));
            cauldronAnimSprite->setPosition(f.position);
            window.draw(*cauldronAnimSprite);
        }
    }
}

bool PlacementSystem::isPlacementValid(const std::string& itemId,
                                      int tx,
                                      int ty,
                                      const House& house,
                                      const World& world,
                                      const Player& player,
                                      const std::vector<Chest>& chests,
                                      const std::vector<Furniture>& furniture,
                                      std::string* outReason) const
{
    sf::Vector2f ppos = player.shape.getPosition();
    sf::Vector2f targetPos = { tx * 32.f, ty * 32.f };

    // 1. Ограничение для предметов интерьера: только внутри дома на деревянном полу!
    if (itemId == "bed" || itemId == "carpet" || itemId == "alchemist_table")
    {
        if (!house.isInside)
        {
            if (outReason) *outReason = "Этот предмет можно ставить только в доме!";
            return false;
        }
    }

    // Размеры и область препятствия
    sf::FloatRect bounds;
    sf::FloatRect obstacle;

    if (itemId == "chest")
    {
        bounds = sf::FloatRect(targetPos, { 32.f, 32.f });
        obstacle = bounds;
    }
    else if (itemId == "bed")
    {
        bounds = sf::FloatRect(targetPos, { 48.f, 82.5f });
        obstacle = sf::FloatRect({ targetPos.x, targetPos.y + 7.5f }, { 48.f, 75.f });
    }
    else if (itemId == "carpet")
    {
        bounds = sf::FloatRect(targetPos, { 64.f, 96.f }); // ровно 2 на 3 тайла
        obstacle = sf::FloatRect({ 0.f, 0.f }, { 0.f, 0.f }); // ковер проходим
    }
    else if (itemId == "alchemist_table")
    {
        bounds = sf::FloatRect(targetPos, { 48.f, 64.f });
        obstacle = sf::FloatRect({ targetPos.x, targetPos.y + 24.f }, { 48.f, 40.f });
    }
    else if (itemId == "cauldron")
    {
        bounds = sf::FloatRect(targetPos, { 36.f, 40.f });
        obstacle = sf::FloatRect({ targetPos.x, targetPos.y + 10.f }, { 36.f, 30.f });
    }
    else
    {
        return false;
    }

    // 2. Проверка выхода за границы локации
    if (house.isInside)
    {
        if (bounds.position.x < House::ROOM_X ||
            bounds.position.x + bounds.size.x > House::ROOM_X + House::ROOM_W ||
            bounds.position.y < House::ROOM_Y ||
            bounds.position.y + bounds.size.y > House::ROOM_Y + House::ROOM_H)
        {
            if (outReason) *outReason = "Нельзя ставить за пределы пола комнаты!";
            return false;
        }

        // Нельзя перекрывать дверь выхода
        sf::FloatRect doorArea({ house.exitDoorPos.x - 28.f, house.exitDoorPos.y - 18.f }, { 56.f, 24.f });
        if (bounds.findIntersection(doorArea).has_value())
        {
            if (outReason) *outReason = "Нельзя перекрывать выход!";
            return false;
        }
    }
    else
    {
        if (tx < 0 || tx >= World::WIDTH || ty < 0 || ty >= World::HEIGHT)
        {
            return false;
        }

        // Вода
        int checkTilesX = static_cast<int>(std::ceil(bounds.size.x / 32.f));
        int checkTilesY = static_cast<int>(std::ceil(bounds.size.y / 32.f));
        for (int dy = 0; dy < checkTilesY; dy++)
        {
            for (int dx = 0; dx < checkTilesX; dx++)
            {
                int cx = tx + dx;
                int cy = ty + dy;
                if (cx >= 0 && cx < World::WIDTH && cy >= 0 && cy < World::HEIGHT)
                {
                    if (world.map[cy][cx].type == 2) // Вода
                    {
                        if (outReason) *outReason = "Нельзя ставить в воду!";
                        return false;
                    }
                }
            }
        }

        // Препятствия снаружи
        if (obstacle.size.x > 0.f && obstacle.size.y > 0.f)
        {
            for (const auto& obs : world.getObstacles())
            {
                if (obstacle.findIntersection(obs).has_value())
                {
                    if (outReason) *outReason = "Здесь уже есть препятствие!";
                    return false;
                }
            }
            for (const auto& obs : house.getExteriorObstacles())
            {
                if (obstacle.findIntersection(obs).has_value())
                {
                    if (outReason) *outReason = "Нельзя ставить в стены дома!";
                    return false;
                }
            }
        }
    }

    // 3. Твердые объекты не должны ставиться под ноги игрока
    if (obstacle.size.x > 0.f && obstacle.size.y > 0.f)
    {
        sf::FloatRect pHitbox = player.getHitbox(ppos);
        if (obstacle.findIntersection(pHitbox).has_value())
        {
            if (outReason) *outReason = "Нельзя ставить под себя!";
            return false;
        }
    }

    // 4. Проверка пересечения с существующими сундуками и мебелью
    if (obstacle.size.x > 0.f && obstacle.size.y > 0.f)
    {
        for (const auto& c : chests)
        {
            if (c.isInside == house.isInside)
            {
                sf::FloatRect cRect(c.position, { 32.f, 32.f });
                if (obstacle.findIntersection(cRect).has_value())
                {
                    if (outReason) *outReason = "Это место уже занято!";
                    return false;
                }
            }
        }

        for (const auto& f : furniture)
        {
            if (f.isInside == house.isInside && f.id != "carpet")
            {
                auto fObs = f.getObstacle();
                if (obstacle.findIntersection(fObs).has_value())
                {
                    if (outReason) *outReason = "Это место уже занято!";
                    return false;
                }
            }
        }
    }

    return true;
}

bool PlacementSystem::tryPlaceFurnitureAt(const std::string& itemId,
                                         int tx,
                                         int ty,
                                         House& house,
                                         const World& world,
                                         Player& player,
                                         Inventory& inv,
                                         std::vector<Chest>& chests,
                                         std::vector<Furniture>& furniture,
                                         std::vector<FloatingText>& floatingTexts,
                                         const ItemDatabase& itemDB)
{
    std::string reason;
    if (!isPlacementValid(itemId, tx, ty, house, world, player, chests, furniture, &reason))
    {
        if (!reason.empty())
        {
            FloatingText ft;
            ft.text = reason;
            ft.position = { tx * 32.f, ty * 32.f };
            ft.color = sf::Color(255, 120, 120);
            ft.timer = 1.4f;
            floatingTexts.push_back(ft);
        }
        return false;
    }

    sf::Vector2f targetPos = { tx * 32.f, ty * 32.f };

    if (itemId == "chest")
    {
        Chest c;
        c.slotCount = 18;
        c.type = "wooden";
        c.position = targetPos;
        c.isInside = house.isInside;
        chests.push_back(c);
    }
    else
    {
        Furniture f;
        f.id = itemId;
        f.isInside = house.isInside;
        if (itemId == "bed")
        {
            // Origin {16, 55} at 1.5x -> bottom center offset { +24.f, +82.5f }
            f.position = { targetPos.x + 24.f, targetPos.y + 82.5f };
        }
        else
        {
            f.position = targetPos;
        }
        furniture.push_back(f);
    }

    inv.removeItem(itemId, 1);

    FloatingText ft;
    const ItemData* d = itemDB.find(itemId);
    ft.text = "+" + (d ? d->name : itemId) + " установлен!";
    ft.position = targetPos;
    ft.color = sf::Color(140, 255, 160);
    ft.timer = 1.4f;
    floatingTexts.push_back(ft);
    return true;
}

bool PlacementSystem::tryPlaceFurniture(const std::string& itemId,
                                       House& house,
                                       const World& world,
                                       Player& player,
                                       Inventory& inv,
                                       std::vector<Chest>& chests,
                                       std::vector<Furniture>& furniture,
                                       std::vector<FloatingText>& floatingTexts,
                                       const ItemDatabase& itemDB)
{
    sf::Vector2i tt = player.getTargetTileCoords();
    int tx = tt.x;
    int ty = tt.y;
    return tryPlaceFurnitureAt(itemId, tx, ty, house, world, player, inv, chests, furniture, floatingTexts, itemDB);
}

bool PlacementSystem::tryPickupFurniture(const House& house,
                                        Player& player,
                                        Inventory& inv,
                                        std::vector<Chest>& chests,
                                        std::vector<Furniture>& furniture,
                                        std::vector<FloatingText>& floatingTexts,
                                        const ItemDatabase& itemDB,
                                        Chest*& nearbyChest,
                                        int& nearbyFurnitureIdx,
                                        std::vector<DroppedItem>* droppedItems)
{
    sf::Vector2f ppos = player.shape.getPosition();
    sf::Vector2f checkPos = { ppos.x + 16.f + player.dirX * 28.f, ppos.y + 22.f + player.dirY * 28.f };

    // Проверяем таймер серии ударов: если прошло больше 1.2 секунды, серия сбрасывается
    if (hitTimer.getElapsedTime().asSeconds() > 1.2f)
    {
        hitCount = 0;
        hitTargetType = 0;
    }

    // 1. Проверяем сундуки в текущей локации
    for (size_t i = 0; i < chests.size(); i++)
    {
        auto& c = chests[i];
        if (c.isInside == house.isInside)
        {
            sf::FloatRect bounds(c.position, { 32.f, 32.f });
            if (bounds.contains(checkPos) || (bounds.position.x - 16.f <= checkPos.x && checkPos.x <= bounds.position.x + bounds.size.x + 16.f &&
                                              bounds.position.y - 16.f <= checkPos.y && checkPos.y <= bounds.position.y + bounds.size.y + 16.f))
            {
                if (c.type == "shipping_bin")
                {
                    FloatingText ft;
                    ft.text = "Ящик сбыта товаров закреплён на ферме!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 140, 140);
                    ft.timer = 1.5f;
                    floatingTexts.push_back(ft);
                    return true;
                }

                // Сундук можно забрать только если он пустой
                bool hasItems = false;
                for (int s = 0; s < c.slotCount; s++)
                {
                    if (!c.inventory.getSlot(s).isEmpty())
                    {
                        hasItems = true;
                        break;
                    }
                }
                if (hasItems)
                {
                    FloatingText ft;
                    ft.text = "Сначала освободите сундук!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 120, 120);
                    ft.timer = 1.3f;
                    floatingTexts.push_back(ft);
                    return true;
                }

                // Проверка того же сундука
                if (hitTargetType != 1 || hitTargetIndex != i)
                {
                    hitCount = 0;
                    hitTargetType = 1;
                    hitTargetIndex = i;
                }

                hitCount++;
                hitTimer.restart();

                if (hitCount < 3)
                {
                    FloatingText ft;
                    ft.text = "Удар по сундуку (" + std::to_string(hitCount) + "/3)";
                    ft.position = c.position;
                    ft.color = sf::Color(255, 205, 90);
                    ft.timer = 0.9f;
                    floatingTexts.push_back(ft);
                    return true;
                }

                // 3 удара достигнуты - убираем сундук!
                hitCount = 0;
                hitTargetType = 0;

                bool added = inv.addItem("chest", 1, 99);
                if (!added && droppedItems)
                {
                    droppedItems->push_back(DroppedItem{ Item{ "chest", 1 }, ppos, 0.f });
                }
                chests.erase(chests.begin() + i);
                nearbyChest = nullptr;

                FloatingText ft;
                ft.text = added ? "Сундук убран в инвентарь!" : "Сундук выпал на землю!";
                ft.position = ppos;
                ft.color = sf::Color(255, 230, 130);
                ft.timer = 1.3f;
                floatingTexts.push_back(ft);
                return true;
            }
        }
    }

    // 2. Проверяем мебель в текущей локации
    for (size_t i = 0; i < furniture.size(); i++)
    {
        auto& f = furniture[i];
        if (f.isInside == house.isInside)
        {
            sf::FloatRect bounds = f.getBounds();
            sf::FloatRect hitBox({ bounds.position.x - 12.f, bounds.position.y - 12.f },
                                 { bounds.size.x + 24.f, bounds.size.y + 24.f });
            if (hitBox.contains(checkPos))
            {
                if (hitTargetType != 2 || hitTargetIndex != i)
                {
                    hitCount = 0;
                    hitTargetType = 2;
                    hitTargetIndex = i;
                }

                hitCount++;
                hitTimer.restart();

                const ItemData* d = itemDB.find(f.id);
                std::string fName = (d ? d->name : f.id);

                if (hitCount < 3)
                {
                    FloatingText ft;
                    ft.text = fName + ": Удар (" + std::to_string(hitCount) + "/3)";
                    ft.position = { bounds.position.x, bounds.position.y };
                    ft.color = sf::Color(255, 205, 90);
                    ft.timer = 0.9f;
                    floatingTexts.push_back(ft);
                    return true;
                }

                // 3 удара достигнуты - убираем мебель!
                hitCount = 0;
                hitTargetType = 0;

                std::string pickedId = f.id;
                bool added = inv.addItem(pickedId, 1, 99);
                if (!added && droppedItems)
                {
                    droppedItems->push_back(DroppedItem{ Item{ pickedId, 1 }, ppos, 0.f });
                }
                furniture.erase(furniture.begin() + i);
                nearbyFurnitureIdx = -1;

                FloatingText ft;
                ft.text = added ? (fName + " убран в инвентарь!") : (fName + " выпал на землю!");
                ft.position = ppos;
                ft.color = sf::Color(255, 230, 130);
                ft.timer = 1.3f;
                floatingTexts.push_back(ft);
                return true;
            }
        }
    }

    return false;
}

void PlacementSystem::drawPlacementGrid(sf::RenderWindow& window,
                                      const Inventory& inv,
                                      int activeSlot,
                                      const Player& player,
                                      const House& house,
                                      const World& world,
                                      const std::vector<Chest>& chests,
                                      const std::vector<Furniture>& furniture,
                                      sf::Vector2i mousePix)
{
    const Item& actItm = inv.getSlot(activeSlot);
    bool isHoldingFurniture = (actItm.id == "chest" || actItm.id == "bed" ||
                               actItm.id == "alchemist_table" || actItm.id == "cauldron" ||
                               actItm.id == "carpet");
    if (!isHoldingFurniture) return;

    sf::Vector2i tc = player.getTileCoords();
    int ptx = tc.x;
    int pty = tc.y;

    // 1. Отрисовка клеток сетки в радиусе 3 тайлов
    sf::RectangleShape cell({ 32.f, 32.f });
    cell.setFillColor(sf::Color(255, 255, 255, 12));
    cell.setOutlineThickness(1.f);

    for (int dy = -3; dy <= 3; dy++)
    {
        for (int dx = -3; dx <= 3; dx++)
        {
            int tx = ptx + dx;
            int ty = pty + dy;

            if (house.isInside)
            {
                if (tx * 32.f < House::ROOM_X || tx * 32.f >= House::ROOM_X + House::ROOM_W ||
                    ty * 32.f < House::ROOM_Y || ty * 32.f >= House::ROOM_Y + House::ROOM_H)
                {
                    continue;
                }
            }
            else
            {
                if (tx < 0 || tx >= World::WIDTH || ty < 0 || ty >= World::HEIGHT)
                {
                    continue;
                }
            }

            cell.setPosition({ tx * 32.f, ty * 32.f });
            cell.setOutlineColor(sf::Color(255, 255, 255, 45));
            window.draw(cell);
        }
    }

    // 2. Индикатор под курсором мыши
    int htx = mousePix.x / 32;
    int hty = mousePix.y / 32;

    int wTiles = 1;
    int hTiles = 1;
    if (actItm.id == "bed" || actItm.id == "carpet") { wTiles = 2; hTiles = 3; }
    else if (actItm.id == "alchemist_table" || actItm.id == "cauldron") { wTiles = 2; hTiles = 2; }

    if (std::abs(htx - ptx) <= 3 && std::abs(hty - pty) <= 3)
    {
        bool valid = isPlacementValid(actItm.id, htx, hty, house, world, player, chests, furniture);

        for (int dy = 0; dy < hTiles; dy++)
        {
            for (int dx = 0; dx < wTiles; dx++)
            {
                sf::RectangleShape hCell({ 32.f, 32.f });
                hCell.setPosition({ (htx + dx) * 32.f, (hty + dy) * 32.f });
                if (valid)
                {
                    hCell.setFillColor(sf::Color(40, 220, 60, 75));
                    hCell.setOutlineColor(sf::Color(80, 255, 100, 210));
                }
                else
                {
                    hCell.setFillColor(sf::Color(220, 40, 40, 75));
                    hCell.setOutlineColor(sf::Color(255, 80, 80, 210));
                }
                hCell.setOutlineThickness(1.5f);
                window.draw(hCell);
            }
        }

        // Полупрозрачный силуэт предмета
        if (valid)
        {
            if (actItm.id == "chest" && chestSprite)
            {
                chestSprite->setPosition({ htx * 32.f, hty * 32.f });
                chestSprite->setColor(sf::Color(255, 255, 255, 150));
                window.draw(*chestSprite);
                chestSprite->setColor(sf::Color::White);
            }
            else if (actItm.id == "carpet" && carpetSprite)
            {
                carpetSprite->setPosition({ htx * 32.f, hty * 32.f });
                carpetSprite->setColor(sf::Color(255, 255, 255, 150));
                window.draw(*carpetSprite);
                carpetSprite->setColor(sf::Color::White);
            }
            else if (actItm.id == "alchemist_table" && tableSprite)
            {
                tableSprite->setTextureRect(sf::IntRect({ 0, 0 }, { 48, 64 }));
                tableSprite->setPosition({ htx * 32.f, hty * 32.f });
                tableSprite->setColor(sf::Color(255, 255, 255, 150));
                window.draw(*tableSprite);
                tableSprite->setColor(sf::Color::White);
            }
            else if (actItm.id == "cauldron" && cauldronAnimSprite)
            {
                cauldronAnimSprite->setTextureRect(sf::IntRect({ 0, 0 }, { 36, 40 }));
                cauldronAnimSprite->setPosition({ htx * 32.f, hty * 32.f });
                cauldronAnimSprite->setColor(sf::Color(255, 255, 255, 150));
                window.draw(*cauldronAnimSprite);
                cauldronAnimSprite->setColor(sf::Color::White);
            }
            else if (actItm.id == "bed" && bedSprite)
            {
                bedSprite->setPosition({ htx * 32.f + 24.f, hty * 32.f + 82.5f });
                bedSprite->setColor(sf::Color(255, 255, 255, 150));
                window.draw(*bedSprite);
                bedSprite->setColor(sf::Color::White);
            }
        }
    }

    // 3. Индикатор перед персонажем (для быстрой установки на [E])
    int frontTx = ptx + player.dirX;
    int frontTy = pty + player.dirY;
    sf::RectangleShape frontCell({ 32.f, 32.f });
    frontCell.setPosition({ frontTx * 32.f, frontTy * 32.f });
    frontCell.setFillColor(sf::Color(255, 255, 0, 30));
    frontCell.setOutlineColor(sf::Color(255, 255, 100, 160));
    frontCell.setOutlineThickness(1.5f);
    window.draw(frontCell);
}
