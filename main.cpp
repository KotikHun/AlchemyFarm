#include <SFML/Graphics.hpp>
#include <optional>
#include <array>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Alchemy Farm");

    // ============================================
    // КАРТА
    // ============================================
    const int MAP_WIDTH = 10;
    const int MAP_HEIGHT = 10;
    const float TILE_SIZE = 32.f;

    // 0 = трава, 1 = грядка
    std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT> map = { {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    } };

    // Один прямоугольник — переиспользуем для всех тайлов
    sf::RectangleShape tileShape({ TILE_SIZE, TILE_SIZE });

    // Цвета тайлов
    const sf::Color GRASS_COLOR(60, 130, 60);      // зелёный
    const sf::Color SOIL_COLOR(139, 90, 43);       // коричневый

    // ============================================
    // ИГРОК
    // ============================================
    const float PLAYER_SIZE = 24.f;
    const float PLAYER_SPEED = 150.f;

    sf::RectangleShape player({ PLAYER_SIZE, PLAYER_SIZE });
    player.setFillColor(sf::Color(70, 130, 220));
    player.setPosition({ 100.f, 100.f });

    sf::Clock clock;

    // ============================================
    // ИГРОВОЙ ЦИКЛ
    // ============================================
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        // События
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // ============================================
        // ОБНОВЛЕНИЕ
        // ============================================
        sf::Vector2f pos = player.getPosition();
        float dx = 0.f;
        float dy = 0.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dy -= PLAYER_SPEED * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dy += PLAYER_SPEED * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dx -= PLAYER_SPEED * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dx += PLAYER_SPEED * dt;

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

        // Рисуем карту — двойной цикл
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                // Позиция тайла в пикселях
                float px = x * TILE_SIZE;
                float py = y * TILE_SIZE;

                tileShape.setPosition({ px, py });

                // Выбираем цвет по типу тайла
                if (map[y][x] == 0)
                    tileShape.setFillColor(GRASS_COLOR);
                else if (map[y][x] == 1)
                    tileShape.setFillColor(SOIL_COLOR);

                window.draw(tileShape);
            }
        }

        // Игрок поверх карты
        window.draw(player);

        window.display();
    }

    return 0;
}