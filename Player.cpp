#include "Player.hpp"

// ============================================
// КОНСТРУКТОР
// ============================================
Player::Player()
{
    shape.setSize({ SIZE, SIZE });
    shape.setFillColor(sf::Color(70, 130, 220));  // синий
    shape.setPosition({ 100.f, 100.f });            // стартовая позиция
}

// ============================================
// ОБНОВЛЕНИЕ (движение)
// ============================================
void Player::update(float dt, float windowWidth, float windowHeight,
    const std::vector<sf::FloatRect>& obstacles)
{
    // Текущая позиция
    sf::Vector2f pos = shape.getPosition();

    // Смещение за этот кадр
    float dx = 0.f;
    float dy = 0.f;

    // WASD — движение и запоминание направления
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        dy -= SPEED * dt;
        dirX = 0;
        dirY = -1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        dy += SPEED * dt;
        dirX = 0;
        dirY = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        dx -= SPEED * dt;
        dirX = -1;
        dirY = 0;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        dx += SPEED * dt;
        dirX = 1;
        dirY = 0;
    }

    // === Проверка коллизий ===
    // Сначала двигаем по X
    sf::Vector2f newPosX = { pos.x + dx, pos.y };
    sf::FloatRect rectX(newPosX, { SIZE, SIZE });
    bool blockedX = false;
    for (const auto& obs : obstacles)
    {
        if (rectX.findIntersection(obs).has_value())
        {
            blockedX = true;
            break;
        }
    }
    if (!blockedX)
        pos.x = newPosX.x;

    // Потом по Y
    sf::Vector2f newPosY = { pos.x, pos.y + dy };
    sf::FloatRect rectY(newPosY, { SIZE, SIZE });
    bool blockedY = false;
    for (const auto& obs : obstacles)
    {
        if (rectY.findIntersection(obs).has_value())
        {
            blockedY = true;
            break;
        }
    }
    if (!blockedY)
        pos.y = newPosY.y;

    // Ограничение по краям окна
    float maxX = windowWidth - SIZE;
    float maxY = windowHeight - SIZE;

    if (pos.x < 0.f) pos.x = 0.f;
    if (pos.y < 0.f) pos.y = 0.f;
    if (pos.x > maxX) pos.x = maxX;
    if (pos.y > maxY) pos.y = maxY;

    shape.setPosition(pos);
}
// ============================================
// ОТРИСОВКА
// ============================================
void Player::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}