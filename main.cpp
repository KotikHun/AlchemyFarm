#include <SFML/Graphics.hpp>
#include <optional>
#include <array>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Alchemy Farm");

    // ============================================
 // КАРТА
 // ============================================
    const int MAP_WIDTH = 30;
    const int MAP_HEIGHT = 25;
    const float TILE_SIZE = 32.f;

    // 0 = трава, 1 = грядка
    std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT> map;

    // Заполняем всю карту травой
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            map[y][x] = 0;
        }
    }

    sf::RectangleShape tileShape({ TILE_SIZE, TILE_SIZE });

    const sf::Color GRASS_COLOR(60, 130, 60);
    const sf::Color SOIL_COLOR(139, 90, 43);

    // ============================================
    // ИГРОК
    // ============================================
    const float PLAYER_SIZE = 24.f;
    const float PLAYER_SPEED = 150.f;

    sf::RectangleShape player({ PLAYER_SIZE, PLAYER_SIZE });
    player.setFillColor(sf::Color(70, 130, 220));
    player.setPosition({ 100.f, 100.f });

    // Направление взгляда игрока (0, -1 = вверх)
    int dirX = 0;
    int dirY = -1;

    // Маркер — куда сейчас нацелен игрок (для отладки/визуализации)
    sf::RectangleShape targetMarker({ TILE_SIZE, TILE_SIZE });
    targetMarker.setFillColor(sf::Color(255, 255, 0, 100));  // полупрозрачный жёлтый

    sf::Clock clock;

    // ============================================
    // ИГРОВОЙ ЦИКЛ
    // ============================================
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        // ============================================
        // СОБЫТИЯ
        // ============================================
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Одноразовое нажатие клавиши
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::E)
                {
                    // === ВЗАИМОДЕЙСТВИЕ ===

                    // 1. Центр игрока
                    sf::Vector2f pos = player.getPosition();
                    float centerX = pos.x + PLAYER_SIZE / 2.f;
                    float centerY = pos.y + PLAYER_SIZE / 2.f;

                    // 2. Тайл под игроком
                    int tileX = static_cast<int>(centerX / TILE_SIZE);
                    int tileY = static_cast<int>(centerY / TILE_SIZE);

                    // 3. Тайл перед игроком
                    int targetX = tileX + dirX;
                    int targetY = tileY + dirY;

                    // 4. Проверка: не выходит ли за границы карты?
                    if (targetX >= 0 && targetX < MAP_WIDTH &&
                        targetY >= 0 && targetY < MAP_HEIGHT)
                    {
                        // 5. Вскопать: трава (0) → грядка (1)
                        if (map[targetY][targetX] == 0)
                        {
                            map[targetY][targetX] = 1;
                        }
                    }
                }
            }
        }

        // ============================================
        // ОБНОВЛЕНИЕ
        // ============================================
        sf::Vector2f pos = player.getPosition();
        float dx = 0.f;
        float dy = 0.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { dy -= PLAYER_SPEED * dt; dirX = 0; dirY = -1; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { dy += PLAYER_SPEED * dt; dirX = 0; dirY = 1; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { dx -= PLAYER_SPEED * dt; dirX = -1; dirY = 0; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { dx += PLAYER_SPEED * dt; dirX = 1; dirY = 0; }

        pos.x += dx;
        pos.y += dy;

        float maxX = 800.f - PLAYER_SIZE;
        float maxY = 600.f - PLAYER_SIZE;

        if (pos.x < 0.f) pos.x = 0.f;
        if (pos.y < 0.f) pos.y = 0.f;
        if (pos.x > maxX) pos.x = maxX;
        if (pos.y > maxY) pos.y = maxY;

        player.setPosition(pos);

        // ============================================
        // ОТРИСОВКА
        // ============================================
        window.clear(sf::Color::Black);

        // Карта
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                float px = x * TILE_SIZE;
                float py = y * TILE_SIZE;

                tileShape.setPosition({ px, py });

                if (map[y][x] == 0)
                    tileShape.setFillColor(GRASS_COLOR);
                else if (map[y][x] == 1)
                    tileShape.setFillColor(SOIL_COLOR);

                window.draw(tileShape);
            }
        }

        // Маркер на тайле перед игроком
        {
            sf::Vector2f ppos = player.getPosition();
            float cx = ppos.x + PLAYER_SIZE / 2.f;
            float cy = ppos.y + PLAYER_SIZE / 2.f;
            int tx = static_cast<int>(cx / TILE_SIZE) + dirX;
            int ty = static_cast<int>(cy / TILE_SIZE) + dirY;

            if (tx >= 0 && tx < MAP_WIDTH && ty >= 0 && ty < MAP_HEIGHT)
            {
                targetMarker.setPosition({ tx * TILE_SIZE, ty * TILE_SIZE });
                window.draw(targetMarker);
            }
        }

        window.draw(player);

        window.display();
    }

    return 0;
}