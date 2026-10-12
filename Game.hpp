#pragma once

#include <SFML/Graphics.hpp>
#include "World.hpp"
#include "Player.hpp"
#include "Inventory.hpp"
#include "UI.hpp"
#include "ItemDatabase.hpp"
#include "DroppedItem.hpp"
#include <vector>
#include "Chest.hpp"

// ============================================
// ИГРА
// ============================================
// Хранит окно, все системы и игровой цикл.

struct Game {
    // Окно
    sf::RenderWindow window;

    // База предметов
    ItemDatabase itemDB;

    // Системы
    World world;
    Player player;
    Inventory inv;
    UI ui;

    // Активный слот хотбара (0-8)
    int activeSlot = 0;
    // Открыт ли инвентарь
    bool isInventoryOpen = false;

    // Состояние перетаскивания
    bool isDragging = false;
    Item draggedItem;
    int draggedFromSlot = -1;
    bool draggedFromHotbar = false;

    // Предметы на земле
    std::vector<DroppedItem> droppedItems;

    // Сундуки на карте
    std::vector<Chest> chests;

    // Ближайший сундук (для открытия по E)
    Chest* nearbyChest = nullptr;

    // Маркер тайла перед игроком
    sf::RectangleShape targetMarker;

    // День и время
    static constexpr float DAY_LENGTH = 60.f;
    float dayTimer = 0.f;
    int currentDay = 1;

    // Часы
    sf::Clock clock;

    // Конструктор — создаёт окно, маркер
    Game();

    // Игровой цикл
    void run();

private:
    // Обработка событий
    void handleEvents();

    // Обновление (день, игрок)
    void update(float dt);

    // Отрисовка (мир, игрок, UI)
    void render();

    // Поиск слота под мышью (возвращает индекс 0-35 или -1)
    int getSlotUnderMouse(sf::Vector2i mousePos);
};