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

            tileShape.setPosition({ px, py });

            Tile& t = map[y][x];

            // Цвет тайла зависит от типа и полива
            if (t.type == 0)
            {
                // 8 оттенков зелёного (разброс шире)
                sf::Color colors[8] = {
                    sf::Color(48, 110, 48),
                    sf::Color(58, 125, 58),
                    sf::Color(68, 140, 68),
                    sf::Color(78, 150, 78),
                    sf::Color(52, 118, 55),
                    sf::Color(62, 132, 62),
                    sf::Color(72, 145, 72),
                    sf::Color(85, 155, 80)
                };
                tileShape.setFillColor(colors[t.variant % 8]);
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

            // Чёрточки травы (только для травы, не для грядки)
                        // Узоры травы (только для травы)
            if (t.type == 0)
            {
                sf::RectangleShape blade({ 2.f, 7.f });
                blade.setFillColor(sf::Color(30, 80, 30));

                sf::CircleShape dot(2.f);
                dot.setFillColor(sf::Color(30, 80, 30));

                // Разные узоры по variant (0-7)
                switch (t.variant % 8)
                {
                case 0:  // 3 вертикальные чёрточки
                    blade.setPosition({ px + 6.f, py + 14.f }); window.draw(blade);
                    blade.setPosition({ px + 14.f, py + 10.f }); window.draw(blade);
                    blade.setPosition({ px + 22.f, py + 16.f }); window.draw(blade);
                    break;

                case 1:  // 2 чёрточки + точка
                    blade.setPosition({ px + 8.f, py + 12.f }); window.draw(blade);
                    blade.setPosition({ px + 20.f, py + 18.f }); window.draw(blade);
                    dot.setPosition({ px + 26.f, py + 8.f }); window.draw(dot);
                    break;

                case 2:  // 1 чёрточка + 2 точки
                    blade.setPosition({ px + 15.f, py + 14.f }); window.draw(blade);
                    dot.setPosition({ px + 6.f, py + 20.f }); window.draw(dot);
                    dot.setPosition({ px + 24.f, py + 12.f }); window.draw(dot);
                    break;

                case 3:  // 4 маленькие точки
                    dot.setPosition({ px + 8.f, py + 10.f }); window.draw(dot);
                    dot.setPosition({ px + 20.f, py + 10.f }); window.draw(dot);
                    dot.setPosition({ px + 8.f, py + 22.f }); window.draw(dot);
                    dot.setPosition({ px + 20.f, py + 22.f }); window.draw(dot);
                    break;

                case 4:  // 2 длинные чёрточки
                    blade.setSize({ 2.f, 10.f });
                    blade.setPosition({ px + 10.f, py + 12.f }); window.draw(blade);
                    blade.setPosition({ px + 20.f, py + 16.f }); window.draw(blade);
                    break;

                case 5:  // 1 чёрточка + 1 точка
                    blade.setPosition({ px + 14.f, py + 16.f }); window.draw(blade);
                    dot.setPosition({ px + 24.f, py + 12.f }); window.draw(dot);
                    break;

                case 6:  // 3 диагональные чёрточки (наклон)
                    blade.setRotation(sf::degrees(30.f));
                    blade.setPosition({ px + 6.f, py + 16.f }); window.draw(blade);
                    blade.setPosition({ px + 16.f, py + 12.f }); window.draw(blade);
                    blade.setPosition({ px + 24.f, py + 18.f }); window.draw(blade);
                    blade.setRotation(sf::degrees(0.f));  // сброс
                    break;

                case 7:  // пусто (чистая трава)
                    break;
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