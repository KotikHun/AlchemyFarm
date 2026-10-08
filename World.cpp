#include "World.hpp"

// ============================================
// КОНСТРУКТОР
// ============================================
World::World()
{
    // Настраиваем tileShape
    tileShape.setSize({ TILE_SIZE, TILE_SIZE });

    // Заполняем карту травой
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            map[y][x].type = 0;
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

            tileShape.setPosition({ px, py });

            Tile& t = map[y][x];

            // Цвет тайла зависит от типа и полива
            if (t.type == 0)
            {
                tileShape.setFillColor(sf::Color(GRASS_R, GRASS_G, GRASS_B));
            }
            else if (t.type == 1)
            {
                if (t.watered)
                {
                    tileShape.setFillColor(sf::Color(WATERED_SOIL_R, WATERED_SOIL_G, WATERED_SOIL_B));
                }
                else
                {
                    tileShape.setFillColor(sf::Color(SOIL_R, SOIL_G, SOIL_B));
                }
            }

            window.draw(tileShape);

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