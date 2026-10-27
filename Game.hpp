#pragma once

#include <SFML/Graphics.hpp>
#include "World.hpp"
#include "Player.hpp"
#include "Inventory.hpp"
#include "UI.hpp"
#include "ItemDatabase.hpp"
#include "DroppedItem.hpp"
#include "FloatingText.hpp"
#include "Chest.hpp"
#include "TimeSystem.hpp"
#include "House.hpp"
#include "AlchemySystem.hpp"
#include "Furniture.hpp"
#include <vector>

enum class InventoryTab {
    Backpack,
    Crafting,
    Journal
};

// ============================================
// ИГРА
// ============================================
// Хранит окно, все системы и игровой цикл.

struct Game {
    sf::RenderWindow window;
    ItemDatabase itemDB;

    World world;
    Player player;
    Inventory inv;
    UI ui;

    // Новые модульные системы
    TimeSystem timeSystem;
    House house;
    AlchemySystem alchemy;

    // Активный слот хотбара (0-8)
    int activeSlot = 0;
    bool isInventoryOpen = false;
    InventoryTab currentTab = InventoryTab::Backpack;
    int journalCategory = 0; // 0 = Зелья, 1 = Растения, 2 = Ремесло

    // Состояние перетаскивания
    bool isDragging = false;
    Item draggedItem;
    int draggedFromSlot = -1;
    bool draggedFromHotbar = false;
    bool draggedFromChest = false;

    // Объекты и мебель на карте (улица и дом)
    std::vector<DroppedItem> droppedItems;
    std::vector<Chest> chests;
    std::vector<Furniture> furniture;

    sf::Texture chestTexture;
    std::unique_ptr<sf::Sprite> chestSprite;

    sf::Texture bedTexture;
    std::unique_ptr<sf::Sprite> bedSprite;

    sf::Texture tableTexture;
    std::unique_ptr<sf::Sprite> tableSprite;

    sf::Texture cauldronAnimTexture;
    std::unique_ptr<sf::Sprite> cauldronAnimSprite;

    sf::Texture carpetTexture;
    std::unique_ptr<sf::Sprite> carpetSprite;

    sf::Texture windowTexture;
    std::unique_ptr<sf::Sprite> windowSprite;

    Chest* nearbyChest = nullptr;
    int nearbyFurnitureIdx = -1;

    // Текстовая подсказка взаимодействия над головой персонажа (без фона)
    std::string playerPrompt;
    sf::RectangleShape targetMarker;

    // Подробный осмотр предмета при зажатии ПКМ
    std::string inspectedItemId;

    // Всплывающие уведомления о предмете в руке и подборе
    std::string activeItemNotification;
    float activeNotificationTimer = 0.f;
    std::vector<FloatingText> floatingTexts;

    sf::Clock clock;

    Game();
    void run();

private:
    void handleEvents();
    void update(float dt);
    void render();
    int getSlotUnderMouse(sf::Vector2i mousePos);
    int getChestSlotUnderMouse(sf::Vector2i mousePos);
    void triggerActiveSlotNotification();
    void sleepInBed(bool fullRest);
    bool isPlacementValid(const std::string& itemId, int tx, int ty, std::string* outReason = nullptr) const;
    bool tryPlaceFurnitureAt(const std::string& itemId, int tx, int ty);
    bool tryPlaceFurniture(const std::string& itemId);
    bool tryPickupFurniture();
    void drawPlacementGrid(sf::RenderWindow& window);
    void drawFurniturePiece(sf::RenderWindow& window, const Furniture& f);
    void transferToChest(Inventory& srcInv, int srcSlot, Chest& chest, const ItemDatabase& itemDB);
    void transferToPlayer(Chest& chest, int chestSlot, Inventory& playerInv, const ItemDatabase& itemDB);
};
