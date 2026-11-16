#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

// ============================================
// ДЕЙСТВИЯ ИГРОКА
// ============================================
enum class PlayerAction {
    None,
    Water,
    Hoe,
    Axe,
    Pickaxe
};

// ============================================
// ИГРОК
// ============================================
// Анимированный персонаж со спрайтами на 4 направления (покой / ходьба / действия)

struct Player {
    static constexpr float SIZE = 32.f;
    static constexpr float SPEED = 160.f;
    static constexpr float ACTION_DURATION = 0.28f;

    // Коллайдер
    sf::RectangleShape shape;

    // Хитбокс у ног персонажа (для плавного перемещения между препятствиями)
    sf::FloatRect getHitbox(const sf::Vector2f& pos) const {
        return sf::FloatRect({ pos.x + 7.f, pos.y + 18.f }, { 18.f, 12.f });
    }

    // Центр основания/ног персонажа на земле
    sf::Vector2f getGroundPosition() const {
        sf::Vector2f pos = shape.getPosition();
        return { pos.x + SIZE / 2.f, pos.y + 22.f };
    }

    // Координаты тайла, на котором стоит персонаж
    sf::Vector2i getTileCoords() const {
        sf::Vector2f gp = getGroundPosition();
        return { static_cast<int>(gp.x / 32.f), static_cast<int>(gp.y / 32.f) };
    }

    // Координаты целевого тайла перед персонажем (по направлению взгляда)
    sf::Vector2i getTargetTileCoords() const {
        sf::Vector2i tc = getTileCoords();
        return { tc.x + dirX, tc.y + dirY };
    }

    // Графика
    sf::Texture texture;
    sf::Texture actionsTexture;
    std::unique_ptr<sf::Sprite> sprite;
    std::unique_ptr<sf::Sprite> actionSprite;

    // Направление взгляда
    int dirX = 0;
    int dirY = 1;  // по умолчанию вниз

    // Состояние анимации перемещения
    bool isMoving = false;
    float animTimer = 0.f;

    // Состояние действий инструментами
    bool isActing = false;
    float actionTimer = 0.f;
    PlayerAction currentAction = PlayerAction::None;

    // Энергия (стамина)
    float energy = 100.f;
    float maxEnergy = 100.f;
    bool isExhausted = false;

    // Кошелек игрока (золото)
    int gold = 100;

    // Эффекты зелий
    float speedBuffTimer = 0.f;
    bool chamomileProtection = false;
    float regenTimer = 0.f;
    float regenRate = 0.f;

    Player();

    void playAction(PlayerAction action);
    void useEnergy(float amount);
    void restoreEnergy(float amount);

    void update(float dt, float windowWidth, float windowHeight,
        const std::vector<sf::FloatRect>& obstacles);

    void draw(sf::RenderWindow& window);
};

