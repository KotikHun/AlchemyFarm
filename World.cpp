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