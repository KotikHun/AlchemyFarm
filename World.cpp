#include "World.hpp"
#include <iostream>
#include <cstdlib>
#include <cmath>

// ============================================
// КОНСТРУКТОР
// ============================================
World::World()
{
    // 1. Трава
    if (!grassTexture.loadFromFile("assets/sprites/grass.png"))
    {
        std::cerr << "World: cannot load grass.png" << std::endl;
    }
    grassSprite = std::make_unique<sf::Sprite>(grassTexture);
    grassSprite->setScale({ 2.f, 2.f });

    // 2. Грядка
    if (!farmlandTexture.loadFromFile("assets/sprites/farmland_tile.png"))
    {
        if (!farmlandTexture.loadFromFile("assets/sprites/farmland.png"))
        {
            std::cerr << "World: cannot load farmland texture" << std::endl;
        }
    }
    farmlandSprite = std::make_unique<sf::Sprite>(farmlandTexture);
    farmlandSprite->setScale({ 2.f, 2.f });

    // 3. Вода (тайлсет с берегами)
    if (!waterTexture.loadFromFile("assets/sprites/water_tile.png"))
    {
        std::cerr << "World: cannot load water_tile.png" << std::endl;
    }
    waterSprite = std::make_unique<sf::Sprite>(waterTexture);
    waterSprite->setScale({ 2.f, 2.f });

    // 4. Декорации (камни, сорняки, пни)
    if (!decorTexture.loadFromFile("assets/sprites/outdoor_decor.png"))
    {
        std::cerr << "World: cannot load outdoor_decor.png" << std::endl;
    }
    decorSprite = std::make_unique<sf::Sprite>(decorTexture);
    decorSprite->setScale({ 2.f, 2.f });

    // 5. Деревья
    if (!treeTexture.loadFromFile("assets/sprites/tree.png"))
    {
        std::cerr << "World: cannot load tree.png" << std::endl;
    }
    treeSprite = std::make_unique<sf::Sprite>(treeTexture);
    treeSprite->setScale({ 2.f, 2.f });

    // 6. Посевы (4 стадии)
    if (!cropsTexture.loadFromFile("assets/sprites/crops.png"))
    {
        std::cerr << "World: cannot load crops.png" << std::endl;
    }
    cropsSprite = std::make_unique<sf::Sprite>(cropsTexture);
    cropsSprite->setScale({ 2.f, 2.f });

    // Генерация карты
    generateNewMap(0);
}

// ============================================
// ГЕНЕРАЦИЯ КАРТЫ
// ============================================
void World::generateNewMap(int seed)
{
    // Очистка старых объектов и деревьев
    objects.clear();
    trees.clear();

    // 1. Инициализация тайлов травы
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            map[y][x].type = 0; // трава
            map[y][x].watered = false;
            map[y][x].plantStage = 0;
            map[y][x].daysGrown = 0;
            int hash = ((x + seed * 13) * 73856093) ^ ((y + seed * 17) * 19349663);
            map[y][x].variant = (hash & 0x7FFFFFFF) % 8;
        }
    }

    // 2. Водоем (пруд)
    for (int y = 10; y <= 14; y++)
    {
        for (int x = 17; x <= 22; x++)
        {
            map[y][x].type = 2; // вода
        }
    }

    // 3. Деревья на ферме (ровно по тайловой сетке, ствол занимает ровно 1 тайл)
    trees.push_back(Tree::createAtTile(3, 17, 4));   // Слева внизу
    trees.push_back(Tree::createAtTile(24, 5, 4));   // Справа вверху
    trees.push_back(Tree::createAtTile(26, 13, 4));  // Справа у края
    trees.push_back(Tree::createAtTile(14, 20, 4));  // Снизу по центру
    trees.push_back(Tree::createAtTile(23, 19, 4));  // Справа внизу

    // 4. Зарезервированная зона: Дом + радиус строго 2 клеток (x in 0..12, y in 0..11)
    // А также водоем: на воде и берегах ничего стоять не может!
    auto inReservedZone = [&](int x, int y) {
        if (x <= 12 && y <= 11) return true; // Чистая зона дома и 2 клеток вокруг него
        if (x >= 16 && x <= 23 && y >= 9 && y <= 15) return true; // Вода и берега: на воде ничего стоять не может!
        for (const auto& tr : trees)
        {
            sf::FloatRect tb = tr.getTrunkBounds();
            int tx = static_cast<int>(std::round(tb.position.x / TILE_SIZE));
            int ty = static_cast<int>(std::round(tb.position.y / TILE_SIZE));
            if (x == tx && y == ty)
                return true;
        }
        return false;
    };

    // 5. Объекты расчистки (камни, сорняки)
    for (int y = 1; y < 24; y++)
    {
        for (int x = 1; x < 29; x++)
        {
            if (map[y][x].type != 0) continue; // На воде и грядках ничего не спавнится!
            if (inReservedZone(x, y)) continue;

            int h = (((x + seed * 7) * 45211) ^ ((y + seed * 19) * 982451)) & 0x7FFFFFFF;
            int r = h % 100;

            if (r < 8)
            {
                WorldObject rock;
                rock.tileX = x;
                rock.tileY = y;
                rock.type = ObjectType::Rock;
                rock.variant = (h / 7) % 2; // Строго 2 варианта серых камней
                rock.hp = 1;
                rock.maxHp = 1;
                objects.push_back(rock);
            }
            else if (r < 20)
            {
                WorldObject weed;
                weed.tileX = x;
                weed.tileY = y;
                weed.type = ObjectType::Weed;
                weed.hp = 1;
                weed.maxHp = 1;
                weed.variant = (h / 13) % 3;
                objects.push_back(weed);
            }
        }
    }
}

// ============================================
// АВТОТАЙЛИНГ ГРЯДКИ
// ============================================
sf::IntRect World::getFarmlandRect(int x, int y) const
{
    bool up    = (y > 0 && map[y - 1][x].type == 1);
    bool down  = (y < HEIGHT - 1 && map[y + 1][x].type == 1);
    bool left  = (x > 0 && map[y][x - 1].type == 1);
    bool right = (x < WIDTH - 1 && map[y][x + 1].type == 1);

    int col = 1;
    if (!left && right) col = 0;
    else if (left && !right) col = 2;
    else if (!left && !right) col = 1;

    int row = 1;
    if (!up && down) row = 0;
    else if (up && !down) row = 2;
    else if (!up && !down) row = 1;

    return sf::IntRect({ col * 16, row * 16 }, { 16, 16 });
}

// ============================================
// АВТОТАЙЛИНГ БЕРЕГОВ ВОДЫ
// ============================================
sf::IntRect World::getWaterRect(int x, int y) const
{
    bool up    = (y > 0 && map[y - 1][x].type == 2);
    bool down  = (y < HEIGHT - 1 && map[y + 1][x].type == 2);
    bool left  = (x > 0 && map[y][x - 1].type == 2);
    bool right = (x < WIDTH - 1 && map[y][x + 1].type == 2);

    int col = 1;
    if (!left && right) col = 0;
    else if (left && !right) col = 2;
    else if (!left && !right) col = 1;

    int row = 1;
    if (!up && down) row = 0;
    else if (up && !down) row = 2;
    else if (!up && !down) row = 1;

    return sf::IntRect({ col * 16, row * 16 }, { 16, 16 });
}

// ============================================
// ОТРИСОВКА
// ============================================
void World::draw(sf::RenderWindow& window)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            float px = x * TILE_SIZE;
            float py = y * TILE_SIZE;

            Tile& t = map[y][x];

            if (t.type == 0)
            {
                grassSprite->setPosition({ px, py });
                window.draw(*grassSprite);

                // Декоративные ростки и цветочки (без камней!)
                if (t.variant == 1)
                {
                    decorSprite->setTextureRect(sf::IntRect({ 48, 16 }, { 16, 16 }));
                    decorSprite->setPosition({ px, py });
                    window.draw(*decorSprite);
                }
                else if (t.variant == 2)
                {
                    decorSprite->setTextureRect(sf::IntRect({ 64, 16 }, { 16, 16 }));
                    decorSprite->setPosition({ px, py });
                    window.draw(*decorSprite);
                }
            }
            else if (t.type == 1)
            {
                farmlandSprite->setTextureRect(getFarmlandRect(x, y));
                farmlandSprite->setPosition({ px, py });
                window.draw(*farmlandSprite);

                if (t.watered)
                {
                    sf::RectangleShape waterOverlay({ TILE_SIZE, TILE_SIZE });
                    waterOverlay.setPosition({ px, py });
                    waterOverlay.setFillColor(sf::Color(30, 30, 60, 90));
                    window.draw(waterOverlay);
                }
            }
            else if (t.type == 2)
            {
                // Сначала рисуем траву, чтобы берега бесшовно ложились на окружающий газон
                grassSprite->setPosition({ px, py });
                window.draw(*grassSprite);

                // Водоем с автоматическими берегами
                sf::IntRect wRect = getWaterRect(x, y);
                waterSprite->setTextureRect(wRect);
                waterSprite->setPosition({ px, py });
                window.draw(*waterSprite);

                // Рябь только на глубокой центральной воде
                if (wRect.position.x == 16 && wRect.position.y == 16)
                {
                    sf::RectangleShape ripple({ TILE_SIZE, TILE_SIZE });
                    ripple.setPosition({ px, py });
                    ripple.setFillColor(sf::Color(255, 255, 255, 25));
                    window.draw(ripple);
                }
            }

            // Отрисовка растения (4 стадии роста)
            if (t.type == 1 && t.plantStage > 0 && cropsSprite)
            {
                int col = 0;
                if (t.plantStage == 1) col = 0;      // 1: Семена
                else if (t.plantStage == 2) col = 1; // 2: Маленький росток
                else if (t.plantStage == 3) col = 3; // 3: Куст с бутонами
                else if (t.plantStage == 4) col = 5; // 4: Спелые ягоды

                cropsSprite->setTextureRect(sf::IntRect({ col * 16, 0 }, { 16, 32 }));
                // Растение 32x64 (16x32 * 2x), смещаем вверх так чтобы низ попадал на грядку
                cropsSprite->setPosition({ px, py - 32.f });
                window.draw(*cropsSprite);
            }
        }
    }

    // Камни, пни и сорняки
    for (const auto& obj : objects)
    {
        sf::IntRect texRect;

        if (obj.type == ObjectType::Rock)
        {
            // Строго два варианта серых камней с травинками по бокам (изображения 2 и 3)
            if (obj.variant == 0)
                texRect = sf::IntRect({ 16, 32 }, { 16, 16 }); // Одиночный гладкий серый камень
            else
                texRect = sf::IntRect({ 32, 32 }, { 16, 16 }); // Слоистый серый камень с травинками
        }
        else if (obj.type == ObjectType::Stump)
        {
            // Натуральный коричневый древесный спил (никаких желтых камней/руды)
            texRect = sf::IntRect({ 0, 112 }, { 16, 16 });
        }
        else
        {
            if (obj.variant == 0) texRect = sf::IntRect({ 0, 16 }, { 16, 16 });
            else if (obj.variant == 1) texRect = sf::IntRect({ 16, 16 }, { 16, 16 });
            else texRect = sf::IntRect({ 32, 16 }, { 16, 16 });
        }

        decorSprite->setTextureRect(texRect);
        decorSprite->setPosition({ obj.tileX * TILE_SIZE, obj.tileY * TILE_SIZE });
        window.draw(*decorSprite);
    }
}

// ============================================
// ДЕРЕВЬЯ
// ============================================
void World::drawTrees(sf::RenderWindow& window, const Player& player)
{
    if (!treeSprite) return;

    sf::Vector2f playerPos = player.shape.getPosition();
    float pcx = playerPos.x + Player::SIZE / 2.f;
    float pcy = playerPos.y + Player::SIZE / 2.f;

    for (const auto& tr : trees)
    {
        treeSprite->setPosition(tr.position);

        if (tr.isBehindCrown(pcx, pcy))
        {
            treeSprite->setColor(sf::Color(255, 255, 255, 125)); // 50% прозрачность
        }
        else
        {
            treeSprite->setColor(sf::Color(255, 255, 255, 255));
        }

        window.draw(*treeSprite);
    }
}

// ============================================
// РОСТ ПРИ НОВОМ ДНЕ
// ============================================
void World::updateGrowth()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            Tile& t = map[y][x];

            // Политые растения растут (до 4 стадии)
            if (t.watered && t.plantStage > 0 && t.plantStage < 4)
            {
                t.plantStage++;
            }
            t.watered = false;
        }
    }
}

// ============================================
// ПОИСК И УДАЛЕНИЕ ОБЪЕКТОВ
// ============================================
WorldObject* World::getObjectAt(int tx, int ty)
{
    for (auto& obj : objects)
    {
        if (obj.tileX == tx && obj.tileY == ty)
            return &obj;
    }
    return nullptr;
}

bool World::removeObjectAt(int tx, int ty)
{
    for (auto it = objects.begin(); it != objects.end(); ++it)
    {
        if (it->tileX == tx && it->tileY == ty)
        {
            objects.erase(it);
            return true;
        }
    }
    return false;
}

// ============================================
// ПРЕПЯТСТВИЯ
// ============================================
std::vector<sf::FloatRect> World::getObstacles() const
{
    std::vector<sf::FloatRect> obs;

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (map[y][x].type == 2)
            {
                obs.push_back(sf::FloatRect({ x * TILE_SIZE, y * TILE_SIZE }, { TILE_SIZE, TILE_SIZE }));
            }
        }
    }

    for (const auto& obj : objects)
    {
        if (obj.isSolid())
        {
            obs.push_back(obj.getBounds(TILE_SIZE));
        }
    }

    for (const auto& tr : trees)
    {
        obs.push_back(tr.getTrunkBounds());
    }

    return obs;
}

// ============================================
// ВЗАИМОДЕЙСТВИЕ
// ============================================
void World::interact(const Player& player, const std::string& itemId, Inventory& inv, std::vector<DroppedItem>& droppedItems)
{
    sf::Vector2f pos = player.shape.getPosition();
    float centerX = pos.x + Player::SIZE / 2.f;
    float centerY = pos.y + Player::SIZE / 2.f;

    int tileX = static_cast<int>(centerX / TILE_SIZE);
    int tileY = static_cast<int>(centerY / TILE_SIZE);

    int targetX = tileX + player.dirX;
    int targetY = tileY + player.dirY;

    if (targetX < 0 || targetX >= WIDTH || targetY < 0 || targetY >= HEIGHT)
    {
        return;
    }

    float targetCenterX = targetX * TILE_SIZE + TILE_SIZE / 2.f;
    float targetCenterY = targetY * TILE_SIZE + TILE_SIZE / 2.f;

    // 1. Рубка дерева (строго по тайлу основания ствола)
    for (auto it = trees.begin(); it != trees.end(); ++it)
    {
        sf::FloatRect trunk = it->getTrunkBounds();
        int trunkTx = static_cast<int>(std::round(trunk.position.x / TILE_SIZE));
        int trunkTy = static_cast<int>(std::round(trunk.position.y / TILE_SIZE));

        if (targetX == trunkTx && targetY == trunkTy)
        {
            if (itemId == "axe")
            {
                it->hp--;
                sf::Vector2f dropPos = { trunk.position.x, trunk.position.y };
                // Свежие листья опадают с кроны дерева при каждом ударе
                droppedItems.push_back(DroppedItem{ Item{ "leaves", 1 }, { dropPos.x + (std::rand() % 28 - 14.f), dropPos.y + (std::rand() % 16 - 8.f) }, 0.f });

                if (it->hp <= 0)
                {
                    droppedItems.push_back(DroppedItem{ Item{ "wood", 4 }, dropPos, 0.f });
                    droppedItems.push_back(DroppedItem{ Item{ "leaves", 3 }, { dropPos.x - 10.f, dropPos.y + 6.f }, 0.f });
                    droppedItems.push_back(DroppedItem{ Item{ "seeds", 1 }, { dropPos.x + 12.f, dropPos.y - 8.f }, 0.f });
                    trees.erase(it);
                }
            }
            return;
        }
    }

    // 2. Камни, пни и сорняки
    WorldObject* obj = getObjectAt(targetX, targetY);
    if (obj != nullptr)
    {
        sf::Vector2f dropPos = { targetX * TILE_SIZE + 4.f, targetY * TILE_SIZE + 4.f };

        if (obj->type == ObjectType::Rock)
        {
            if (itemId == "pickaxe")
            {
                obj->hp--;
                if (obj->hp <= 0)
                {
                    int dropCount = (obj->variant == 3) ? 2 : 1;
                    removeObjectAt(targetX, targetY);
                    droppedItems.push_back(DroppedItem{ Item{ "stone", dropCount }, dropPos, 0.f });
                }
            }
            return;
        }
        else if (obj->type == ObjectType::Stump)
        {
            if (itemId == "axe")
            {
                obj->hp--;
                if (obj->hp <= 0)
                {
                    removeObjectAt(targetX, targetY);
                    droppedItems.push_back(DroppedItem{ Item{ "wood", 2 }, dropPos, 0.f });
                }
            }
            return;
        }
        else if (obj->type == ObjectType::Weed)
        {
            if (itemId == "hoe" || itemId == "axe" || itemId.empty())
            {
                obj->hp--;
                if (obj->hp <= 0)
                {
                    removeObjectAt(targetX, targetY);
                    droppedItems.push_back(DroppedItem{ Item{ "fiber", 1 }, dropPos, 0.f });
                    if (std::rand() % 2 == 0)
                    {
                        droppedItems.push_back(DroppedItem{ Item{ "leaves", 1 }, { dropPos.x + 8.f, dropPos.y - 4.f }, 0.f });
                    }
                    if (std::rand() % 3 == 0)
                    {
                        droppedItems.push_back(DroppedItem{ Item{ "seeds", 1 }, dropPos, 0.f });
                    }
                }
            }
            return;
        }
    }

    // 3. Действие с грядкой / землей
    Tile& t = map[targetY][targetX];

    if (t.type == 2)
    {
        return; // вода
    }

    if (itemId == "hoe")
    {
        if (t.type == 0)
        {
            t.type = 1; // вскопка
        }
        else if (t.type == 1 && t.plantStage > 0)
        {
            if (t.plantStage == 4)
            {
                // Сбор урожая
                droppedItems.push_back(DroppedItem{ Item{ "berry", 2 }, { targetX * TILE_SIZE + 4.f, targetY * TILE_SIZE + 4.f }, 0.f });
            }
            t.plantStage = 0;
            t.watered = false;
        }
    }
    else if (itemId == "seeds")
    {
        if (t.type == 1 && t.plantStage == 0 && inv.countItem("seeds") > 0)
        {
            t.plantStage = 1; // Посадка (Стадия 1: семена)
            inv.removeItem("seeds", 1);
        }
    }
    else if (itemId == "watering_can")
    {
        if (t.type == 1 && !t.watered)
        {
            t.watered = true; // Полив
        }
    }
    else if (itemId.empty())
    {
        // Рука: сбор спелого урожая
        if (t.type == 1 && t.plantStage == 4)
        {
            droppedItems.push_back(DroppedItem{ Item{ "berry", 2 }, { targetX * TILE_SIZE + 4.f, targetY * TILE_SIZE + 4.f }, 0.f });
            t.plantStage = 0;
            t.watered = false;
        }
    }
}
