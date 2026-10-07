#include <SFML/Graphics.hpp>
#include <optional>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Alchemy Farm");

    // ============================================
    // ГРЯДКА
    // ============================================
    const float TILE_SIZE = 32.f;

    sf::RectangleShape tile({ TILE_SIZE, TILE_SIZE });
    tile.setFillColor(sf::Color(139, 90, 43));
    tile.setPosition({ 384.f, 284.f });

    // ============================================
    // ИГРОК
    // ============================================
    const float PLAYER_SIZE = 24.f;
    const float PLAYER_SPEED = 150.f;  // пикселей в секунду

    sf::RectangleShape player({ PLAYER_SIZE, PLAYER_SIZE });
    player.setFillColor(sf::Color(70, 130, 220));  // синий
    player.setPosition({ 100.f, 100.f });             // левый верхний угол окна

    // Часы для измерения времени между кадрами
    sf::Clock clock;

    // ============================================
    // ИГРОВОЙ ЦИКЛ
    // ============================================
    while (window.isOpen())
    {
        // Время между кадрами (в секундах)
        float dt = clock.restart().asSeconds();

        // Обработка событий
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // ============================================
        // ОБНОВЛЕНИЕ (логика)
        // ============================================

        // Текущая позиция игрока
        sf::Vector2f pos = player.getPosition();

        // Смещение за этот кадр
        float dx = 0.f;
        float dy = 0.f;

        // Читаем клавиши WASD
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dy -= PLAYER_SPEED * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dy += PLAYER_SPEED * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dx -= PLAYER_SPEED * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dx += PLAYER_SPEED * dt;

        // Применяем смещение
        pos.x += dx;
        pos.y += dy;

        // Ограничение по краям окна
        float maxX = 800.f - PLAYER_SIZE;
        float maxY = 600.f - PLAYER_SIZE;

        if (pos.x < 0.f) pos.x = 0.f;
        if (pos.y < 0.f) pos.y = 0.f;
        if (pos.x > maxX) pos.x = maxX;
        if (pos.y > maxY) pos.y = maxY;

        // Записываем позицию обратно в игрока
        player.setPosition(pos);

        // ============================================
        // ОТРИСОВКА
        // ============================================
        window.clear(sf::Color::Black);
        window.draw(tile);
        window.draw(player);   // рисуем игрока ПОВЕРХ грядки
        window.display();
    }

    return 0;
}