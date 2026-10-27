#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Player.hpp"

// ============================================
// ДОМ ИГРОКА: 2 НЕЗАВИСИМЫЕ СЦЕНЫ (ЛОКАЦИИ)
// ============================================

class House {
public:
    // --- ЛОКАЦИЯ 1: «ВНЕШНИЙ МИР / УЛИЦА» ---
    // Позиция дома на сетке фермы (Bottom Center Pivot)
    // Размеры дома 9x9 тайлов (288x288px при масштабе 2.0x)
    sf::Vector2f exteriorPos = { 208.f, 320.f };

    // Спрайт экстерьера (Pack 5 1 вариант)
    sf::Texture exteriorTexture;
    std::unique_ptr<sf::Sprite> exteriorSprite;

    // --- ЛОКАЦИЯ 2: «ИНТЕРЬЕР ДОМА» (ОТДЕЛЬНАЯ СЦЕНА) ---
    bool isInside = false;

    // Спрайт пола (темный паркет из ассетов)
    sf::Texture floorTexture;
    std::unique_ptr<sf::Sprite> floorSprite;

    // Спрайт кровати (ТОЛЬКО ОДНА КРОВАТЬ, черная из ассетов)
    sf::Texture bedTexture;
    std::unique_ptr<sf::Sprite> bedSprite;

    // Координаты отдельной сцены интерьера (уютная комната 12x8 тайлов)
    static constexpr float ROOM_X = 288.f;
    static constexpr float ROOM_Y = 192.f;
    static constexpr float ROOM_W = 384.f;
    static constexpr float ROOM_H = 256.f;

    // Позиция кровати в верхнем углу пола (Pivot: Bottom Center)
    sf::Vector2f bedPos = { ROOM_X + 40.f, ROOM_Y + 85.f };

    // Зона выхода (внутренняя дверь)
    sf::Vector2f exitDoorPos = { ROOM_X + ROOM_W / 2.f, ROOM_Y + ROOM_H - 12.f };

    House();

    void update(float dt);

    // Отрисовка дома снаружи на ферме (Локация 1)
    void drawExterior(sf::RenderWindow& window);

    // Отрисовка интерьера (Локация 2: отдельная сцена — только пол и одна новая кровать)
    void drawInterior(sf::RenderWindow& window);

    // Физика и коллайдеры
    std::vector<sf::FloatRect> getExteriorObstacles() const;
    std::vector<sf::FloatRect> getInteriorObstacles() const;

    // Триггеры перехода между сценами
    bool isNearExteriorDoor(const sf::Vector2f& playerPos) const;
    bool isNearExitDoor(const sf::Vector2f& playerPos) const;
    bool isNearBed(const sf::Vector2f& playerPos) const;

    // Переходы (телепорт между сценами)
    void enter(Player& player);
    void exit(Player& player);
};
