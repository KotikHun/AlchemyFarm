#include "World.hpp"
#include <iostream>

// ============================================
// КОНСТРУКТОР
// ============================================
World::World()
{
    // Настраиваем tileShape
    tileShape.setSize({ TILE_SIZE, TILE_SIZE });

    // Загружаем спрайт травы
    if (!grassTexture.loadFromFile("assets/sprites/grass.png"))
    {
        std::cerr << "World: cannot load grass.png" << std::endl;
    }
    grassSprite = std::make_unique<sf::Sprite>(grassTexture);
    grassSprite->setScale({ 2.f, 2.f });

    // Загружаем спрайт грядки
    if (!farmlandTexture.loadFromFile("assets/sprites/farmland.png"))
    {
        std::cerr << "World: cannot load farmland.png" << std::endl;
    }
    farmlandSprite = std::make_unique<sf::Sprite>(farmlandTexture);
    farmlandSprite->setScale({ 32.f / 48.f, 32.f / 48.f });  // 48×48 → 32×32

    // Заполняем карту травой
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            map[y][x].type = 0;

            // Псевдослучайный хэш от (x, y) — выглядит случайно, но стабильно
            // (тот же тайл всегда с тем же вариантом)
            int hash = (x * 73856093) ^ (y * 19349663);
            map[y][x].variant = (hash & 0x7FFFFFFF) % 8;  // 0-7
        }
    }
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
            }
            else if (t.type == 1)
            {
                // Грядка — спрайт
                farmlandSprite->setPosition({ px, py });
                window.draw(*farmlandSprite);

                // Политая грядка — тёмный полупрозрачный слой сверху
                if (t.watered)
                {
                    sf::RectangleShape waterOverlay({ TILE_SIZE, TILE_SIZE });
                    waterOverlay.setPosition({ px, py });
                    waterOverlay.setFillColor(sf::Color(30, 30, 60, 80));  // тёмно-синий, полупрозрачный
                    window.draw(waterOverlay);
                }
            }

            // Растение (если посажено)
            if (t.plantStage > 0)
            {
                float cx = px + TILE_SIZE / 2.f;
                float cy = py + TILE_SIZE / 2.f;

                float radius = 4.f;
                sf::Color plantColor(60, 130, 60);

                if (t.plantStage == 2)
                {
                    radius = 7.f;
                    plantColor = sf::Color(100, 180, 100);
                }
                else if (t.plantStage == 3)
                {
                    radius = 10.f;
                    plantColor = sf::Color(150, 220, 100);
                }

                sf::CircleShape plant(radius);
                plant.setFillColor(plantColor);
                plant.setOrigin({ radius, radius });
                plant.setPosition({ cx, cy });
                window.draw(plant);
            }
        }
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

            // Политые растения растут (до 3 стадии)
            if (t.watered && t.plantStage > 0 && t.plantStage < 3)
            {
                t.plantStage++;
            }

            // Сброс полива — утром надо полить снова
            t.watered = false;
        }
    }
}


// ============================================
// ВЗАИМОДЕЙСТВИЕ
// ============================================
void World::interact(const Player& player, const std::string& itemId, Inventory& inv)
{
    // 1. Центр игрока
    sf::Vector2f pos = player.shape.getPosition();
    float centerX = pos.x + Player::SIZE / 2.f;
    float centerY = pos.y + Player::SIZE / 2.f;

    // 2. Тайл под игроком
    int tileX = static_cast<int>(centerX / TILE_SIZE);
    int tileY = static_cast<int>(centerY / TILE_SIZE);

    // 3. Тайл перед игроком
    int targetX = tileX + player.dirX;
    int targetY = tileY + player.dirY;

    // 4. Проверка границ карты
    if (targetX < 0 || targetX >= WIDTH ||
        targetY < 0 || targetY >= HEIGHT)
    {
        return;
    }

    // 5. Ссылка на тайл
    Tile& t = map[targetY][targetX];

    // 6. Действие зависит от предмета в руке
    if (itemId == "hoe")
    {
        // Мотыга
        if (t.type == 0)
        {
            t.type = 1;
        }
        else if (t.type == 1 && t.plantStage > 0)
        {
            if (t.plantStage == 3)
                inv.addItem("leaves", 1, 99);
            t.plantStage = 0;
            t.watered = false;
        }
    }
    else if (itemId == "seeds")
    {
        // Семечко
        if (t.type == 1 && t.plantStage == 0 && inv.countItem("seeds") > 0)
        {
            t.plantStage = 1;
            inv.removeItem("seeds", 1);
        }
    }
    else if (itemId == "watering_can")
    {
        // Лейка
        if (t.type == 1 && !t.watered)
        {
            t.watered = true;
        }
    }
    else if (itemId.empty())
    {
        // Рука — собрать
        if (t.type == 1 && t.plantStage == 3)
        {
            inv.addItem("leaves", 1, 99);
            t.plantStage = 0;
            t.watered = false;
        }
    }
}