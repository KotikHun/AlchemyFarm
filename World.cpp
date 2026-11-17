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

    // Саженцы дуба
    if (!saplingTexture.loadFromFile("assets/sprites/sapling.png"))
    {
        std::cerr << "World: cannot load sapling.png" << std::endl;
    }
    saplingSprite = std::make_unique<sf::Sprite>(saplingTexture);

    // 6. Посевы ягод (4 стадии)
    if (!cropsTexture.loadFromFile("assets/sprites/crops.png"))
    {
        std::cerr << "World: cannot load crops.png" << std::endl;
    }
    cropsSprite = std::make_unique<sf::Sprite>(cropsTexture);
    cropsSprite->setScale({ 2.f, 2.f });

    // 7. Посевы ромашки (4 стадии: семена -> New Piskel 5 -> New Piskel 6 -> цветущая ромашка)
    if (!chamomileTexture.loadFromFile("assets/sprites/crops/chamomile/chamomile_strip.png"))
    {
        std::cerr << "World: cannot load chamomile_strip.png" << std::endl;
    }
    chamomileSprite = std::make_unique<sf::Sprite>(chamomileTexture);
    chamomileSprite->setScale({ 1.f, 1.f });

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

    // 2. Водоемы (пруд у дома и дикое лесное озеро на юге)
    for (int y = 10; y <= 14; y++)
    {
        for (int x = 17; x <= 22; x++)
        {
            map[y][x].type = 2; // стартовый пруд у дома
        }
    }
    for (int y = 42; y <= 47; y++)
    {
        for (int x = 40; x <= 46; x++)
        {
            map[y][x].type = 2; // южное дикое озеро
        }
    }

    // 3. Деревья на ферме и в лесу (по тайловой сетке)
    // Стартовая ферма
    trees.push_back(Tree::createAtTile(3, 17, 4));   // Слева внизу от дома
    trees.push_back(Tree::createAtTile(24, 5, 4));   // Справа вверху
    trees.push_back(Tree::createAtTile(26, 13, 4));  // Справа у пруда
    trees.push_back(Tree::createAtTile(14, 20, 4));  // Снизу по центру
    trees.push_back(Tree::createAtTile(23, 19, 4));  // Справа внизу

    // Восточный и южный дубовый лес
    trees.push_back(Tree::createAtTile(35, 7, 4));
    trees.push_back(Tree::createAtTile(43, 10, 4));
    trees.push_back(Tree::createAtTile(51, 8, 4));
    trees.push_back(Tree::createAtTile(38, 17, 4));
    trees.push_back(Tree::createAtTile(48, 18, 4));
    trees.push_back(Tree::createAtTile(54, 24, 4));
    trees.push_back(Tree::createAtTile(36, 28, 4));
    trees.push_back(Tree::createAtTile(46, 32, 4));
    trees.push_back(Tree::createAtTile(52, 38, 4));
    trees.push_back(Tree::createAtTile(30, 42, 4));
    trees.push_back(Tree::createAtTile(48, 50, 4));
    trees.push_back(Tree::createAtTile(22, 52, 4));
    trees.push_back(Tree::createAtTile(10, 48, 4));

    // Молодые деревца и саженцы в роще
    trees.push_back(Tree::createAtTile(40, 13, 1));
    trees.push_back(Tree::createAtTile(46, 22, 0));
    trees.push_back(Tree::createAtTile(34, 34, 1));
    trees.push_back(Tree::createAtTile(50, 44, 0));
    trees.push_back(Tree::createAtTile(26, 48, 1));

    // 4. Зарезервированная зона: Дом + радиус 2 клеток (x in 0..12, y in 0..11)
    // А также водоемы: на воде и берегах ничего стоять не может!
    auto inReservedZone = [&](int x, int y) {
        if (x <= 12 && y <= 11) return true; // Чистая зона дома и веранды
        if (x >= 16 && x <= 23 && y >= 9 && y <= 15) return true; // Первый водоем
        if (x >= 39 && x <= 47 && y >= 41 && y <= 48) return true; // Второе озеро
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

    // 5. Объекты расчистки (камни, пни, сорняки)
    for (int y = 1; y < HEIGHT - 1; y++)
    {
        for (int x = 1; x < WIDTH - 1; x++)
        {
            if (map[y][x].type != 0) continue; // На воде и грядках ничего не спавнится
            if (inReservedZone(x, y)) continue;

            int h = (((x + seed * 7) * 45211) ^ ((y + seed * 19) * 982451)) & 0x7FFFFFFF;
            int r = h % 100;

            // Зона каменистого карьера на юго-западе (x in 4..16, y in 32..44)
            bool isQuarry = (x >= 4 && x <= 16 && y >= 32 && y <= 44);

            if (isQuarry)
            {
                if (r < 22)
                {
                    WorldObject rock;
                    rock.tileX = x;
                    rock.tileY = y;
                    rock.type = ObjectType::Rock;
                    rock.variant = (h / 7) % 2;
                    rock.hp = 1;
                    rock.maxHp = 1;
                    objects.push_back(rock);
                }
                else if (r < 27)
                {
                    WorldObject stump;
                    stump.tileX = x;
                    stump.tileY = y;
                    stump.type = ObjectType::Stump;
                    stump.hp = 1;
                    stump.maxHp = 1;
                    objects.push_back(stump);
                }
                else if (r < 35)
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
            else
            {
                if (r < 7)
                {
                    WorldObject rock;
                    rock.tileX = x;
                    rock.tileY = y;
                    rock.type = ObjectType::Rock;
                    rock.variant = (h / 7) % 2;
                    rock.hp = 1;
                    rock.maxHp = 1;
                    objects.push_back(rock);
                }
                else if (r < 17)
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
    // Frustum Culling: вычисляем видимую область камеры для оптимизации
    sf::View view = window.getView();
    sf::Vector2f center = view.getCenter();
    sf::Vector2f size = view.getSize();

    float left = center.x - size.x / 2.f;
    float top = center.y - size.y / 2.f;
    float right = center.x + size.x / 2.f;
    float bottom = center.y + size.y / 2.f;

    int startX = std::max(0, static_cast<int>(std::floor(left / TILE_SIZE)) - 1);
    int endX = std::min(WIDTH - 1, static_cast<int>(std::ceil(right / TILE_SIZE)) + 1);
    int startY = std::max(0, static_cast<int>(std::floor(top / TILE_SIZE)) - 1);
    int endY = std::min(HEIGHT - 1, static_cast<int>(std::ceil(bottom / TILE_SIZE)) + 1);

    for (int y = startY; y <= endY; y++)
    {
        for (int x = startX; x <= endX; x++)
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

            // Отрисовка растения (стадии роста)
            if (t.type == 1 && t.plantStage > 0)
            {
                if (t.cropType == 1 && chamomileSprite)
                {
                    // Ромашка (4 стадии в полоске 128x32, размер кадра 32x32)
                    int frame = std::clamp(t.plantStage - 1, 0, 3);
                    chamomileSprite->setTextureRect(sf::IntRect({ frame * 32, 0 }, { 32, 32 }));
                    chamomileSprite->setPosition({ px, py });
                    window.draw(*chamomileSprite);
                }
                else if (cropsSprite)
                {
                    // Ягоды: 4 стадии роста
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
    }

    // Камни, пни и сорняки (с отсечением невидимых за пределами экрана)
    for (const auto& obj : objects)
    {
        if (obj.tileX < startX || obj.tileX > endX || obj.tileY < startY || obj.tileY > endY)
            continue;

        sf::IntRect texRect;

        if (obj.type == ObjectType::Rock)
        {
            if (obj.variant == 0)
                texRect = sf::IntRect({ 16, 32 }, { 16, 16 });
            else
                texRect = sf::IntRect({ 32, 32 }, { 16, 16 });
        }
        else if (obj.type == ObjectType::Stump)
        {
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
void World::drawSingleTree(sf::RenderWindow& window, const Tree& tr, const Player& player)
{
    if (!treeSprite) return;

    sf::Vector2f playerPos = player.shape.getPosition();
    float pcx = playerPos.x + Player::SIZE / 2.f;
    float pcy = playerPos.y + Player::SIZE / 2.f;

    if (tr.stage == 2)
    {
        treeSprite->setScale({ 2.f, 2.f });
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
    else if (tr.stage == 1)
    {
        // Молодое дерево (средняя стадия): пропорционально меньше взрослого дуба
        treeSprite->setScale({ 1.25f, 1.25f });
        float trunkBaseX = tr.position.x + 48.f + 16.f;
        float trunkBaseY = tr.position.y + 110.f + 32.f;
        treeSprite->setPosition({ trunkBaseX - 40.f, trunkBaseY - 100.f });
        treeSprite->setColor(sf::Color(255, 255, 255, 255));
        window.draw(*treeSprite);
    }
    else // stage == 0: саженец
    {
        if (saplingSprite)
        {
            saplingSprite->setScale({ 1.f, 1.f });
            saplingSprite->setPosition({ tr.position.x + 48.f, tr.position.y + 110.f });
            window.draw(*saplingSprite);
        }
    }
}

void World::drawTrees(sf::RenderWindow& window, const Player& player)
{
    for (const auto& tr : trees)
    {
        drawSingleTree(window, tr, player);
    }
}

void World::drawTreesBehind(sf::RenderWindow& window, const Player& player)
{
    sf::View view = window.getView();
    sf::Vector2f center = view.getCenter();
    sf::Vector2f size = view.getSize();
    float left = center.x - size.x / 2.f - Tree::WIDTH;
    float right = center.x + size.x / 2.f + Tree::WIDTH;
    float top = center.y - size.y / 2.f - Tree::HEIGHT;
    float bottom = center.y + size.y / 2.f + Tree::HEIGHT;

    float pGroundY = player.getGroundPosition().y;
    for (const auto& tr : trees)
    {
        if (tr.position.x < left || tr.position.x > right ||
            tr.position.y < top || tr.position.y > bottom)
        {
            continue;
        }

        float trunkBaseY = tr.position.y + 110.f + 32.f;
        if (trunkBaseY <= pGroundY)
        {
            drawSingleTree(window, tr, player);
        }
    }
}

void World::drawTreesInFront(sf::RenderWindow& window, const Player& player)
{
    sf::View view = window.getView();
    sf::Vector2f center = view.getCenter();
    sf::Vector2f size = view.getSize();
    float left = center.x - size.x / 2.f - Tree::WIDTH;
    float right = center.x + size.x / 2.f + Tree::WIDTH;
    float top = center.y - size.y / 2.f - Tree::HEIGHT;
    float bottom = center.y + size.y / 2.f + Tree::HEIGHT;

    float pGroundY = player.getGroundPosition().y;
    for (const auto& tr : trees)
    {
        if (tr.position.x < left || tr.position.x > right ||
            tr.position.y < top || tr.position.y > bottom)
        {
            continue;
        }

        float trunkBaseY = tr.position.y + 110.f + 32.f;
        if (trunkBaseY > pGroundY)
        {
            drawSingleTree(window, tr, player);
        }
    }
}

// ============================================
// РОСТ ПРИ НОВОМ ДНЕ
// ============================================
void World::updateGrowth()
{
    // 1. Посевы культур на грядках
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            Tile& t = map[y][x];

            // Политые растения растут
            if (t.watered && t.plantStage > 0 && t.plantStage < 4)
            {
                if (t.cropType == 1)
                {
                    // Ромашка: 1 день семена -> 2 день росток -> 3 и 4 день подросший -> 5 день финальный
                    t.daysGrown++;
                    if (t.daysGrown == 1)
                    {
                        t.plantStage = 2; // День 2: росток (Piskel 5)
                    }
                    else if (t.daysGrown == 2)
                    {
                        t.plantStage = 3; // День 3: подросший (Piskel 6)
                    }
                    else if (t.daysGrown == 3)
                    {
                        t.plantStage = 3; // День 4: подросший (Piskel 6)
                    }
                    else if (t.daysGrown >= 4)
                    {
                        t.plantStage = 4; // День 5: финальный (спелая ромашка)
                    }
                }
                else
                {
                    t.daysGrown++;
                    t.plantStage++;
                }
            }
            t.watered = false;
        }
    }

    // 2. Деревья (растут долго: саженец 3 дня -> молодой дуб 4 дня -> взрослый дуб на 7 день)
    for (auto& tr : trees)
    {
        if (tr.stage < 2)
        {
            tr.daysGrown++;
            if (tr.daysGrown >= 3 && tr.stage == 0)
            {
                tr.stage = 1; // переход в молодое дерево
                tr.hp = 2;
            }
            else if (tr.daysGrown >= 7 && tr.stage == 1)
            {
                tr.stage = 2; // полноценный взрослый дуб
                tr.hp = 4;
            }
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
void World::interact(Player& player, const std::string& itemId, Inventory& inv, std::vector<DroppedItem>& droppedItems, bool isRaining)
{
    sf::Vector2i tc = player.getTileCoords();
    int tileX = tc.x;
    int tileY = tc.y;

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

                if (it->hp <= 0)
                {
                    if (it->stage == 2)
                    {
                        // Взрослый дуб: 4 древесины + желуди (1-2 шт)
                        droppedItems.push_back(DroppedItem{ Item{ "wood", 4 }, dropPos, 0.f });
                        int acornCount = (std::rand() % 100 < 50) ? 2 : 1;
                        droppedItems.push_back(DroppedItem{ Item{ "acorn", acornCount }, { dropPos.x + 8.f, dropPos.y - 6.f }, 0.f });
                    }
                    else if (it->stage == 1)
                    {
                        // Молодое дерево: 2 древесины + 1 желудь
                        droppedItems.push_back(DroppedItem{ Item{ "wood", 2 }, dropPos, 0.f });
                        droppedItems.push_back(DroppedItem{ Item{ "acorn", 1 }, dropPos, 0.f });
                    }
                    else
                    {
                        // Саженец: 1 желудь
                        droppedItems.push_back(DroppedItem{ Item{ "acorn", 1 }, dropPos, 0.f });
                    }
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
            if (isRaining)
            {
                t.watered = true; // новая грядка во время дождя сразу становится политой
            }
        }
        else if (t.type == 1 && t.plantStage > 0)
        {
            if (t.plantStage == 4)
            {
                // Сбор урожая
                float dropX = targetX * TILE_SIZE + 4.f;
                float dropY = targetY * TILE_SIZE + 4.f;
                if (t.cropType == 1)
                {
                    droppedItems.push_back(DroppedItem{ Item{ "chamomile", 2 }, { dropX, dropY }, 0.f });
                    if (rand() % 100 < 40)
                    {
                        droppedItems.push_back(DroppedItem{ Item{ "chamomile_seeds", 1 }, { dropX + 8.f, dropY - 8.f }, 0.f });
                    }
                }
                else
                {
                    droppedItems.push_back(DroppedItem{ Item{ "berry", 2 }, { dropX, dropY }, 0.f });
                    if (rand() % 100 < 30)
                    {
                        droppedItems.push_back(DroppedItem{ Item{ "seeds", 1 }, { dropX + 8.f, dropY - 8.f }, 0.f });
                    }
                }
            }
            t.plantStage = 0;
            t.cropType = 0;
            t.watered = false;
            t.daysGrown = 0;
        }
    }
    else if (itemId == "seeds")
    {
        if (t.type == 1 && t.plantStage == 0 && inv.countItem("seeds") > 0)
        {
            t.plantStage = 1; // Посадка (Стадия 1: семена ягод)
            t.cropType = 0;
            t.daysGrown = 0;
            inv.removeItem("seeds", 1);
            player.useEnergy(1.f);
        }
    }
    else if (itemId == "chamomile_seeds")
    {
        if (t.type == 1 && t.plantStage == 0 && inv.countItem("chamomile_seeds") > 0)
        {
            t.plantStage = 1; // Посадка (Стадия 1: семена ромашки)
            t.cropType = 1;
            t.daysGrown = 0;
            inv.removeItem("chamomile_seeds", 1);
            player.useEnergy(1.f);
        }
    }
    else if (itemId == "acorn")
    {
        // Посадка дубового желудя (на свободную землю: траву или грядку)
        if ((t.type == 0 || t.type == 1) && inv.countItem("acorn") > 0)
        {
            bool occupied = (getObjectAt(targetX, targetY) != nullptr);
            for (const auto& tr : trees)
            {
                sf::FloatRect tb = tr.getTrunkBounds();
                int tx = static_cast<int>(std::round(tb.position.x / TILE_SIZE));
                int ty = static_cast<int>(std::round(tb.position.y / TILE_SIZE));
                if (tx == targetX && ty == targetY) { occupied = true; break; }
            }
            if (!occupied)
            {
                Tree sapling = Tree::createAtTile(targetX, targetY, 1, 0); // stage 0, hp 1
                trees.push_back(sapling);
                inv.removeItem("acorn", 1);
                player.useEnergy(1.f);
            }
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
            float dropX = targetX * TILE_SIZE + 4.f;
            float dropY = targetY * TILE_SIZE + 4.f;
            if (t.cropType == 1)
            {
                droppedItems.push_back(DroppedItem{ Item{ "chamomile", 2 }, { dropX, dropY }, 0.f });
                if (rand() % 100 < 40)
                {
                    droppedItems.push_back(DroppedItem{ Item{ "chamomile_seeds", 1 }, { dropX + 8.f, dropY - 8.f }, 0.f });
                }
            }
            else
            {
                droppedItems.push_back(DroppedItem{ Item{ "berry", 2 }, { dropX, dropY }, 0.f });
                if (rand() % 100 < 30)
                {
                    droppedItems.push_back(DroppedItem{ Item{ "seeds", 1 }, { dropX + 8.f, dropY - 8.f }, 0.f });
                }
            }
            t.plantStage = 0;
            t.cropType = 0;
            t.watered = false;
            t.daysGrown = 0;
        }
    }
}
