#include "Player.hpp"
#include <iostream>
#include <cmath>

// ============================================
// КОНСТРУКТОР
// ============================================
Player::Player()
{
    shape.setSize({ SIZE, SIZE });
    shape.setFillColor(sf::Color::Transparent); // прозрачный, так как рисуется спрайт
    shape.setPosition({ 140.f, 140.f });

    // Загрузка спрайтшита персонажа (192x320)
    if (!texture.loadFromFile("assets/sprites/player.png"))
    {
        std::cerr << "Player: cannot load player.png" << std::endl;
    }

    sprite = std::make_unique<sf::Sprite>(texture);
    // Размер кадра: 32x32, точка привязки в районе ног
    sprite->setTextureRect(sf::IntRect({ 0, 0 }, { 32, 32 }));
    sprite->setOrigin({ 16.f, 24.f });
    sprite->setScale({ 2.0f, 2.0f });
    sprite->setPosition({ 140.f + SIZE / 2.f, 140.f + SIZE / 2.f });

    // Загрузка спрайтшита действий (96x576, кадры 48x48)
    if (actionsTexture.loadFromFile("assets/sprites/player_actions.png"))
    {
        actionSprite = std::make_unique<sf::Sprite>(actionsTexture);
        actionSprite->setTextureRect(sf::IntRect({ 0, 0 }, { 48, 48 }));
        actionSprite->setOrigin({ 24.f, 32.f });
        actionSprite->setScale({ 2.0f, 2.0f });
        actionSprite->setPosition({ 140.f + SIZE / 2.f, 140.f + SIZE / 2.f });
    }
    else
    {
        std::cerr << "Player: cannot load player_actions.png" << std::endl;
    }
}

void Player::playAction(PlayerAction action)
{
    currentAction = action;
    isActing = true;
    actionTimer = 0.f;
}

void Player::useEnergy(float amount)
{
    energy -= amount;
    if (energy <= 0.f)
    {
        energy = 0.f;
        isExhausted = true;
    }
}

void Player::restoreEnergy(float amount)
{
    energy += amount;
    if (energy > maxEnergy) energy = maxEnergy;
    if (energy > 15.f) isExhausted = false;
}

// ============================================
// ОБНОВЛЕНИЕ (движение и анимация)
// ============================================
void Player::update(float dt, float windowWidth, float windowHeight,
    const std::vector<sf::FloatRect>& obstacles)
{
    sf::Vector2f pos = shape.getPosition();

    // === Анимация действия инструмента ===
    if (isActing)
    {
        actionTimer += dt;
        if (actionTimer >= ACTION_DURATION)
        {
            isActing = false;
            currentAction = PlayerAction::None;
        }
        else
        {
            int frame = (actionTimer < (ACTION_DURATION / 2.f)) ? 0 : 1;

            int baseRow = 0;
            switch (currentAction)
            {
            case PlayerAction::Pickaxe: baseRow = 0; break;
            case PlayerAction::Hoe:     baseRow = 3; break;
            case PlayerAction::Axe:     baseRow = 6; break;
            case PlayerAction::Water:   baseRow = 9; break;
            default:                    baseRow = 0; break;
            }

            int rowOffset = 0;
            float scaleX = 2.0f;

            if (currentAction == PlayerAction::Water)
            {
                if (dirY > 0)
                {
                    rowOffset = 0; // Лейка: Вниз
                }
                else if (dirY < 0)
                {
                    rowOffset = 2; // Лейка: Вверх
                }
                else
                {
                    rowOffset = 1; // Лейка: Вправо / Влево
                    scaleX = (dirX < 0) ? -2.0f : 2.0f;
                }
            }
            else
            {
                // Для Кирки (строки 0-2), Мотыги (3-5), Топора (6-8):
                // строка 0 = горизонтальный замах, строка 1 = удар вниз, строка 2 = удар вверх
                if (dirY > 0)
                {
                    rowOffset = 1; // Удар вниз
                }
                else if (dirY < 0)
                {
                    rowOffset = 2; // Удар вверх
                }
                else
                {
                    rowOffset = 0; // Удар вправо / влево
                    scaleX = (dirX < 0) ? -2.0f : 2.0f;
                }
            }

            int row = baseRow + rowOffset;

            if (actionSprite)
            {
                actionSprite->setTextureRect(sf::IntRect({ frame * 48, row * 48 }, { 48, 48 }));
                actionSprite->setScale({ scaleX, 2.0f });
                actionSprite->setPosition({ pos.x + SIZE / 2.f, pos.y + SIZE / 2.f + 4.f });
            }

            // Во время замаха/полива персонаж стоит на месте
            return;
        }
    }

    // Таймер баффа скорости
    if (speedBuffTimer > 0.f)
    {
        speedBuffTimer -= dt;
    }

    float currentSpeed = SPEED;
    if (speedBuffTimer > 0.f)
    {
        currentSpeed *= 1.4f; // Бафф от зелья скорости
    }
    else if (isExhausted)
    {
        currentSpeed *= 0.55f; // Замедление при усталости
    }

    float dx = 0.f;
    float dy = 0.f;

    // Ввод WASD
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        dy -= currentSpeed * dt;
        dirX = 0;
        dirY = -1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        dy += currentSpeed * dt;
        dirX = 0;
        dirY = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        dx -= currentSpeed * dt;
        dirX = -1;
        dirY = 0;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        dx += currentSpeed * dt;
        dirX = 1;
        dirY = 0;
    }

    isMoving = (dx != 0.f || dy != 0.f);

    // === Проверка коллизий по X (хитбокс ног) ===
    sf::Vector2f newPosX = { pos.x + dx, pos.y };
    sf::FloatRect rectX = getHitbox(newPosX);
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

    // === Проверка коллизий по Y (хитбокс ног) ===
    sf::Vector2f newPosY = { pos.x, pos.y + dy };
    sf::FloatRect rectY = getHitbox(newPosY);
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

    // Ограничение по краям экрана
    float maxX = windowWidth - SIZE;
    float maxY = windowHeight - SIZE;
    if (pos.x < 0.f) pos.x = 0.f;
    if (pos.y < 0.f) pos.y = 0.f;
    if (pos.x > maxX) pos.x = maxX;
    if (pos.y > maxY) pos.y = maxY;

    shape.setPosition(pos);

    // === Обновление анимации ===
    if (isMoving)
    {
        animTimer += dt * 8.f; // 8 кадров в секунду при ходьбе
    }
    else
    {
        animTimer += dt * 4.f; // 4 кадра в секунду в покое
    }

    int frame = static_cast<int>(animTimer) % 6;

    // Выбор строки спрайтшита:
    // 0: Idle Down, 1: Idle Right, 2: Idle Up
    // 3: Walk Down, 4: Walk Right, 5: Walk Up
    int row = 0;
    if (isMoving)
    {
        if (dirY > 0) row = 3;
        else if (dirY < 0) row = 5;
        else row = 4;
    }
    else
    {
        if (dirY > 0) row = 0;
        else if (dirY < 0) row = 2;
        else row = 1;
    }

    if (sprite)
    {
        sprite->setTextureRect(sf::IntRect({ frame * 32, row * 32 }, { 32, 32 }));

        // Отражение влево при взгляде влево
        float scaleX = (dirX < 0) ? -2.0f : 2.0f;
        sprite->setScale({ scaleX, 2.0f });
        sprite->setPosition({ pos.x + SIZE / 2.f, pos.y + SIZE / 2.f + 4.f });
    }
}

// ============================================
// ОТРИСОВКА
// ============================================
void Player::draw(sf::RenderWindow& window)
{
    if (isActing && actionSprite)
    {
        window.draw(*actionSprite);
    }
    else if (sprite)
    {
        window.draw(*sprite);
    }
    else
    {
        window.draw(shape);
    }
}
