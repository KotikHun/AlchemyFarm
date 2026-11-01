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
#include "SoundManager.hpp"
#include "Furniture.hpp"
#include "SaveManager.hpp"
#include "WeatherSystem.hpp"
#include "QuestManager.hpp"
#include "PlacementSystem.hpp"
#include <vector>

enum class InventoryTab {
    Backpack,
    Crafting,
    Journal
};

enum class GameState {
    MainMenu,
    Loading,
    Playing,
    PauseMenu,
    Sleeping
};

// ============================================
// ИГРА
// ============================================
// Хранит окно, все системы и игровой цикл.

struct Game {
    sf::RenderWindow window;
    ItemDatabase itemDB;

    // Состояние игры и настройки
    GameState gameState = GameState::MainMenu;
    MenuModal currentModal = MenuModal::None;
    GameSettings settings;
    int hoveredMenuButton = -1;
    int hoveredPauseButton = -1;
    int selectedSaveSlot = 0;
    int currentSaveSlot = 1;
    bool isSaveMode = false;
    int pendingDeleteSlot = -1;
    std::vector<SaveSlotInfo> cachedSaveSlots;
    float menuAnimTime = 0.f;
    std::vector<MenuParticle> menuParticles;

    // Отчёт о ночных продажах и сне
    SleepReport sleepReport;
    bool isSleepNextHovered = false;

    // Плавные переходы экрана (Fade in / Fade out)
    float fadeAlpha = 0.f;
    float fadeTarget = 0.f;
    float fadeSpeed = 380.f;
    bool isTransitioningSleep = false;
    bool isTransitioningWake = false;

    // Состояние экрана загрузки
    float loadingProgress = 0.f;
    float loadingTimer = 0.f;
    int currentTipIndex = 0;
    std::string loadingStageText;

    World world;
    Player player;
    Inventory inv;
    UI ui;

    // Новые модульные системы
    TimeSystem timeSystem;
    House house;
    AlchemySystem alchemy;
    SoundManager sound;
    WeatherSystem weather;
    QuestManager quests;
    PlacementSystem placement;

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

    std::unique_ptr<sf::Sprite>& chestSprite;
    std::unique_ptr<sf::Sprite>& bedSprite;
    std::unique_ptr<sf::Sprite>& tableSprite;
    std::unique_ptr<sf::Sprite>& cauldronAnimSprite;
    std::unique_ptr<sf::Sprite>& carpetSprite;
    std::unique_ptr<sf::Sprite>& windowSprite;

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

    // Виртуальный экран и Letterbox (сохранение пропорций 960x800)
    static constexpr float VIRTUAL_WIDTH = 960.f;
    static constexpr float VIRTUAL_HEIGHT = 800.f;
    sf::View letterboxView;
    bool isFullscreen = false;
    void updateLetterboxView(unsigned int windowWidth, unsigned int windowHeight);
    sf::Vector2f getMouseVirtualCoords() const;
    sf::Vector2i getMouseVirtualPixel() const;

    // Погода и квесты (перенаправление в подсистемы)
    std::vector<RainDrop>& rainDrops;
    void initRain() { weather.init(); }
    void updateRain(float dt) { weather.update(dt, timeSystem.isRaining); }
    void drawRain(sf::RenderWindow& window) { weather.draw(window, timeSystem.isRaining, house.isInside, VIRTUAL_WIDTH, VIRTUAL_HEIGHT); }

    std::vector<QuestRecord>& dailyQuests;
    std::vector<ShopCatalogItem>& shopCatalog;
    bool& isOrdersBoardOpen;
    int& ordersBoardTab;
    bool& nearOrdersBoard;
    void initDailyQuests(int day) { quests.initDailyQuests(day); }
    void initShopCatalog() { quests.initShopCatalog(); }
    void handleOrdersBoardEvents(const sf::Event& event) {
        quests.handleEvents(event, getMouseVirtualCoords(), inv, player, sound, floatingTexts);
    }

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
    bool isPlacementValid(const std::string& itemId, int tx, int ty, std::string* outReason = nullptr) const {
        return placement.isPlacementValid(itemId, tx, ty, house, world, player, chests, furniture, outReason);
    }
    bool tryPlaceFurnitureAt(const std::string& itemId, int tx, int ty) {
        return placement.tryPlaceFurnitureAt(itemId, tx, ty, house, world, player, inv, chests, furniture, floatingTexts, itemDB);
    }
    bool tryPlaceFurniture(const std::string& itemId) {
        return placement.tryPlaceFurniture(itemId, house, world, player, inv, chests, furniture, floatingTexts, itemDB);
    }
    bool tryPickupFurniture() {
        return placement.tryPickupFurniture(house, player, inv, chests, furniture, floatingTexts, itemDB, nearbyChest, nearbyFurnitureIdx);
    }
    void drawPlacementGrid(sf::RenderWindow& window) {
        placement.drawPlacementGrid(window, inv, activeSlot, player, house, world, chests, furniture, getMouseVirtualPixel());
    }
    void drawFurniturePiece(sf::RenderWindow& window, const Furniture& f) {
        placement.drawFurniturePiece(window, f);
    }

    void transferToChest(Inventory& srcInv, int srcSlot, Chest& chest, const ItemDatabase& itemDB);
    void transferToPlayer(Chest& chest, int chestSlot, Inventory& playerInv, const ItemDatabase& itemDB);

    // Новые методы главного меню, загрузки и паузы
    void startNewGame();
    void startLoadingGame();
    void updateMenu(float dt);
    void updateLoading(float dt);
    void handleMenuEvents(const sf::Event& event);
    void handleLoadingEvents(const sf::Event& event);
    void handlePauseEvents(const sf::Event& event);
    void initParticles();
    void updateParticles(float dt);

    // Ночной отчёт и сон
    void triggerSleep(bool fullRest);
    void wakeUpFromSleep();
    void handleSleepingEvents(const sf::Event& event);

    // Сохранение и загрузка
    void refreshSaveSlots();
    void saveCurrentGame(int slot);
    void loadGameFromSlot(int slot);
    void deleteGameSlot(int slot);
};
