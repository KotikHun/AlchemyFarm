#include "Game.hpp"
#include <iostream>
#include <cmath>

static bool isInside(float mx, float my, float x, float y, float size)
{
    return mx >= x && mx < x + size && my >= y && my < y + size;
}

static bool isInsideRect(float mx, float my, float x, float y, float w, float h)
{
    return mx >= x && mx < x + w && my >= y && my < y + h;
}

// ============================================
// КОНСТРУКТОР
// ============================================
Game::Game()
    : window(sf::VideoMode({ 960, 800 }), toUtf8("Alchemy Farm")),
      player(),
      weather(),
      quests(),
      placement(),
      rainDrops(weather.rainDrops),
      dailyQuests(quests.dailyQuests),
      shopCatalog(quests.shopCatalog),
      isOrdersBoardOpen(quests.isOpen),
      ordersBoardTab(quests.activeTab),
      nearOrdersBoard(quests.nearBoard),
      chestSprite(placement.chestSprite),
      bedSprite(placement.bedSprite),
      tableSprite(placement.tableSprite),
      cauldronAnimSprite(placement.cauldronAnimSprite),
      carpetSprite(placement.carpetSprite),
      windowSprite(placement.windowSprite)
{
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    // Загрузка базы предметов из JSON
    if (!itemDB.loadFromFile("data/items.json"))
    {
        std::cerr << "Game: cannot load data/items.json" << std::endl;
    }



    // Маркер тайла перед игроком
    targetMarker.setSize({ World::TILE_SIZE, World::TILE_SIZE });
    targetMarker.setFillColor(sf::Color(255, 255, 0, 80));
    targetMarker.setOutlineColor(sf::Color(255, 255, 255, 140));
    targetMarker.setOutlineThickness(1.5f);

    // Частицы для фона главного меню
    initParticles();

    // Начальная генерация мира и стартовых объектов
    startNewGame();

    updateLetterboxView(window.getSize().x, window.getSize().y);
    initRain();
    initShopCatalog();
    initDailyQuests(1);

    // Старт с главного меню
    gameState = GameState::MainMenu;
    currentModal = MenuModal::None;
    refreshSaveSlots();
}

// ============================================
// ИНИЦИАЛИЗАЦИЯ И ОБНОВЛЕНИЕ ЧАСТИЦ МЕНЮ
// ============================================
void Game::initParticles()
{
    menuParticles.clear();
    for (int i = 0; i < 40; i++)
    {
        MenuParticle p;
        p.pos = { static_cast<float>(rand() % 960), static_cast<float>(rand() % 800) };
        p.vel = { ((rand() % 20) - 10) * 1.5f, -15.f - (rand() % 25) };
        p.alpha = 140.f + (rand() % 90);
        p.size = 2.f + (rand() % 3);
        p.maxLife = 3.f + (rand() % 4);
        p.life = ((rand() % 100) / 100.f) * p.maxLife;

        int cType = rand() % 3;
        if (cType == 0) p.color = sf::Color(255, 220, 110);      // Золотистый
        else if (cType == 1) p.color = sf::Color(120, 230, 160); // Травяной
        else p.color = sf::Color(160, 215, 255);                 // Магический синий

        menuParticles.push_back(p);
    }
}

void Game::updateParticles(float dt)
{
    for (auto& p : menuParticles)
    {
        p.pos += p.vel * dt;
        p.pos.x += std::sin(menuAnimTime * 2.f + p.pos.y * 0.05f) * 8.f * dt;
        p.life += dt;
        if (p.life >= p.maxLife || p.pos.y < -15.f)
        {
            p.pos = { static_cast<float>(rand() % 960), 810.f };
            p.vel = { ((rand() % 20) - 10) * 1.5f, -15.f - (rand() % 25) };
            p.life = 0.f;
            p.maxLife = 3.f + (rand() % 4);
        }
    }
}

// ============================================
// ВИРТУАЛЬНЫЙ ЭКРАН И LETTERBOX (СОХРАНЕНИЕ ПРОПОРЦИЙ)
// ============================================
void Game::updateLetterboxView(unsigned int windowWidth, unsigned int windowHeight)
{
    float windowRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    float viewRatio = VIRTUAL_WIDTH / VIRTUAL_HEIGHT;

    float sizeX = 1.0f;
    float sizeY = 1.0f;
    float posX = 0.0f;
    float posY = 0.0f;

    if (windowRatio >= viewRatio)
    {
        // Окно шире -> черные полосы по бокам (pillarbox)
        sizeX = viewRatio / windowRatio;
        posX = (1.0f - sizeX) / 2.0f;
    }
    else
    {
        // Окно выше -> черные полосы сверху и снизу (letterbox)
        sizeY = windowRatio / viewRatio;
        posY = (1.0f - sizeY) / 2.0f;
    }

    letterboxView.setSize({ VIRTUAL_WIDTH, VIRTUAL_HEIGHT });
    letterboxView.setCenter({ VIRTUAL_WIDTH / 2.f, VIRTUAL_HEIGHT / 2.f });
    letterboxView.setViewport(sf::FloatRect({ posX, posY }, { sizeX, sizeY }));
    window.setView(letterboxView);
}

sf::Vector2f Game::getMouseVirtualCoords() const
{
    return window.mapPixelToCoords(sf::Mouse::getPosition(window), letterboxView);
}

sf::Vector2i Game::getMouseVirtualPixel() const
{
    sf::Vector2f c = getMouseVirtualCoords();
    return sf::Vector2i(static_cast<int>(std::round(c.x)), static_cast<int>(std::round(c.y)));
}

// ============================================
// START NEW GAME
// ============================================
void Game::startNewGame()
{
    // 1. Сброс игрока
    player.energy = player.maxEnergy;
    player.isExhausted = false;
    player.isActing = false;
    player.actionTimer = 0.f;
    player.dirX = 0;
    player.dirY = 1;
    player.gold = 100;
    player.shape.setPosition({ house.exteriorPos.x + 32.f - 16.f, house.exteriorPos.y - 48.f });

    // 2. Сброс инвентаря на стартовый набор
    inv = Inventory();
    inv.addItem("hoe", 1, 1);
    inv.addItem("watering_can", 1, 1);
    inv.addItem("axe", 1, 1);
    inv.addItem("pickaxe", 1, 1);
    inv.addItem("seeds", 10, 99);
    inv.addItem("berry", 5, 99);
    inv.addItem("empty_flask", 3, 99);
    inv.addItem("alchemist_table", 1, 99);

    // 3. Сброс времени
    timeSystem = TimeSystem();

    // 4. Сброс дома и интерьера
    house.isInside = false;

    // 5. Генерация новой карты мира
    world.generateNewMap(rand() % 1000);

    // 6. Очистка и установка стартовых сундуков и мебели
    chests.clear();
    furniture.clear();
    droppedItems.clear();
    floatingTexts.clear();

    // Стартовый деревянный сундук на улице (Tile 13, 10)
    Chest chest;
    chest.slotCount = 18;
    chest.type = "wooden";
    chest.position = { 13 * 32.f, 10 * 32.f }; // { 416.f, 320.f }
    chest.isInside = false;
    chests.push_back(chest);

    // Ящик сбыта товаров (Shipping Bin) возле дома (Tile 12, 10)
    Chest shippingBin;
    shippingBin.slotCount = 18;
    shippingBin.type = "shipping_bin";
    shippingBin.position = { 12 * 32.f, 10 * 32.f }; // { 384.f, 320.f }
    shippingBin.isInside = false;
    chests.push_back(shippingBin);

    // Магический котел возле сундука (Tile 14, 10)
    Furniture cauldronObj;
    cauldronObj.id = "cauldron";
    cauldronObj.position = { 14 * 32.f, 10 * 32.f }; // { 448.f, 320.f }
    cauldronObj.isInside = false;
    furniture.push_back(cauldronObj);

    // Внутри дома: кровать и ковер
    Furniture bedObj;
    bedObj.id = "bed";
    bedObj.position = { House::ROOM_X + 32.f, House::ROOM_Y + 85.f };
    bedObj.isInside = true;
    furniture.push_back(bedObj);

    Furniture carpetObj;
    carpetObj.id = "carpet";
    carpetObj.position = { House::ROOM_X + 160.f, House::ROOM_Y + 80.f };
    carpetObj.isInside = true;
    furniture.push_back(carpetObj);

    // 7. Сброс алхимии
    alchemy.isCauldronOpen = false;
    alchemy.isTableOpen = false;
    alchemy.showRecipes = false;

    // 8. Сброс окон и слота
    activeSlot = 0;
    isInventoryOpen = false;
    isDragging = false;
    nearbyChest = nullptr;
    nearbyFurnitureIdx = -1;
    playerPrompt.clear();
    inspectedItemId.clear();

    triggerActiveSlotNotification();
}

// ============================================
// ЗАПУСК ЗАГРУЗКИ НОВОЙ ИГРЫ
// ============================================
void Game::startLoadingGame()
{
    gameState = GameState::Loading;
    loadingProgress = 0.f;
    loadingTimer = 0.f;
    currentTipIndex = rand() % 5;
    loadingStageText = "Инициализация алхимических систем...";

    // Генерация свежего мира и сброс состояния
    startNewGame();
}

void Game::updateMenu(float dt)
{
    menuAnimTime += dt;
    updateParticles(dt);

    sf::Vector2i mpos = getMouseVirtualPixel();
    hoveredMenuButton = -1;

    if (currentModal == MenuModal::None)
    {
        for (int i = 0; i < 5; i++)
        {
            if (UI::getMainMenuButtonRect(i).contains(static_cast<sf::Vector2f>(mpos)))
            {
                hoveredMenuButton = i;
                break;
            }
        }
    }
    else if (currentModal == MenuModal::About)
    {
        if (UI::getAboutBackButtonRect().contains(static_cast<sf::Vector2f>(mpos)))
        {
            hoveredMenuButton = 0; // Для подсветки кнопки Назад в drawAboutModal
        }
    }
}

void Game::updateLoading(float dt)
{
    menuAnimTime += dt;
    loadingTimer += dt;
    float duration = 1.8f;
    loadingProgress = loadingTimer / duration;

    if (loadingProgress < 0.25f)
    {
        loadingStageText = "Очистка поля и вспашка почвы...";
    }
    else if (loadingProgress < 0.50f)
    {
        loadingStageText = "Генерация водоема и лесных зарослей...";
    }
    else if (loadingProgress < 0.75f)
    {
        loadingStageText = "Подготовка инструментов, семян и зелий...";
    }
    else if (loadingProgress < 0.95f)
    {
        loadingStageText = "Обустройство дома и алхимической лаборатории...";
    }
    else
    {
        loadingStageText = "Мир готов! Добро пожаловать на ферму!";
    }

    if (loadingProgress >= 1.0f)
    {
        loadingProgress = 1.0f;
        gameState = GameState::Playing;
    }
}

void Game::handleMenuEvents(const sf::Event& event)
{
    sf::Vector2i mpos = getMouseVirtualPixel();
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    if (const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseBtn->button == sf::Mouse::Button::Left)
        {
            if (currentModal == MenuModal::None)
            {
                for (int i = 0; i < 5; i++)
                {
                    if (UI::getMainMenuButtonRect(i).contains(static_cast<sf::Vector2f>(mpos)))
                    {
                        if (i == 0) // Новая игра -> запуск загрузки
                        {
                            startLoadingGame();
                        }
                        else if (i == 1) // Загрузить игру
                        {
                            currentModal = MenuModal::Saves;
                            isSaveMode = false;
                            pendingDeleteSlot = -1;
                            refreshSaveSlots();
                        }
                        else if (i == 2) // Настройки
                        {
                            currentModal = MenuModal::Settings;
                        }
                        else if (i == 3) // Об игре
                        {
                            currentModal = MenuModal::About;
                        }
                        else if (i == 4) // Выход
                        {
                            window.close();
                        }
                        return;
                    }
                }
            }
            else if (currentModal == MenuModal::Settings)
            {
                // Кнопка звука [ - ]
                if (UI::getSettingsSoundMinusRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    settings.soundVolume = std::max(0, settings.soundVolume - 10);
                }
                // Кнопка звука [ + ]
                else if (UI::getSettingsSoundPlusRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    settings.soundVolume = std::min(100, settings.soundVolume + 10);
                }
                // Кнопка музыки [ - ]
                else if (UI::getSettingsMusicMinusRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    settings.musicVolume = std::max(0, settings.musicVolume - 10);
                }
                // Кнопка музыки [ + ]
                else if (UI::getSettingsMusicPlusRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    settings.musicVolume = std::min(100, settings.musicVolume + 10);
                }
                // Кнопка Назад
                else if (UI::getSettingsBackButtonRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    currentModal = MenuModal::None;
                }
            }
            else if (currentModal == MenuModal::Saves)
            {
                bool handled = false;
                for (int i = 0; i < 3; i++)
                {
                    const SaveSlotInfo& sInfo = (i < static_cast<int>(cachedSaveSlots.size())) ? cachedSaveSlots[i] : SaveSlotInfo{};

                    if (sInfo.exists)
                    {
                        // Кнопка [Загрузить / Записать]
                        if (UI::getSavesActionBtnRect(i).contains(static_cast<sf::Vector2f>(mpos)))
                        {
                            if (isSaveMode)
                            {
                                saveCurrentGame(i + 1);
                                refreshSaveSlots();
                            }
                            else
                            {
                                loadGameFromSlot(i + 1);
                            }
                            handled = true;
                            break;
                        }

                        // Кнопка [Удалить]
                        if (UI::getSavesDeleteBtnRect(i).contains(static_cast<sf::Vector2f>(mpos)))
                        {
                            if (pendingDeleteSlot == (i + 1))
                            {
                                deleteGameSlot(i + 1);
                                refreshSaveSlots();
                                pendingDeleteSlot = -1;
                            }
                            else
                            {
                                pendingDeleteSlot = i + 1;
                                sound.playClick();
                            }
                            handled = true;
                            break;
                        }
                    }
                    else if (isSaveMode)
                    {
                        // Пустой слот: кнопка [Сохранить]
                        if (UI::getSavesEmptyBtnRect(i).contains(static_cast<sf::Vector2f>(mpos)))
                        {
                            saveCurrentGame(i + 1);
                            refreshSaveSlots();
                            handled = true;
                            break;
                        }
                    }
                }

                // Кнопка [ НАЗАД ]
                if (!handled && UI::getSavesBackButtonRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    currentModal = MenuModal::None;
                    pendingDeleteSlot = -1;
                    sound.playClick();
                }
                else if (!handled)
                {
                    pendingDeleteSlot = -1;
                }
            }
            else if (currentModal == MenuModal::About)
            {
                // Кнопка Назад
                if (UI::getAboutBackButtonRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    currentModal = MenuModal::None;
                }
            }
        }
    }
    else if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            if (currentModal != MenuModal::None)
            {
                currentModal = MenuModal::None;
            }
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter)
        {
            if (currentModal == MenuModal::None)
            {
                startLoadingGame();
            }
        }
    }
}

void Game::handleLoadingEvents(const sf::Event& event)
{
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Space ||
            keyPressed->code == sf::Keyboard::Key::Enter)
        {
            loadingProgress = 1.0f;
            gameState = GameState::Playing;
        }
    }
    else if (const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseBtn->button == sf::Mouse::Button::Left)
        {
            loadingProgress = 1.0f;
            gameState = GameState::Playing;
        }
    }
}

void Game::handlePauseEvents(const sf::Event& event)
{
    sf::Vector2i mpos = getMouseVirtualPixel();
    float mx = static_cast<float>(mpos.x);
    float my = static_cast<float>(mpos.y);

    if (const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseBtn->button == sf::Mouse::Button::Left)
        {
            if (currentModal == MenuModal::None)
            {
                for (int i = 0; i < 6; i++)
                {
                    if (UI::getPauseMenuButtonRect(i).contains(static_cast<sf::Vector2f>(mpos)))
                    {
                        sound.playClick();
                        if (i == 0) // Продолжить
                        {
                            gameState = GameState::Playing;
                        }
                        else if (i == 1) // Сохранить игру
                        {
                            currentModal = MenuModal::Saves;
                            isSaveMode = true;
                            pendingDeleteSlot = -1;
                            refreshSaveSlots();
                        }
                        else if (i == 2) // Загрузить игру
                        {
                            currentModal = MenuModal::Saves;
                            isSaveMode = false;
                            pendingDeleteSlot = -1;
                            refreshSaveSlots();
                        }
                        else if (i == 3) // Настройки
                        {
                            currentModal = MenuModal::Settings;
                        }
                        else if (i == 4) // В главное меню
                        {
                            gameState = GameState::MainMenu;
                            currentModal = MenuModal::None;
                            refreshSaveSlots();
                        }
                        else if (i == 5) // Выйти из игры
                        {
                            window.close();
                        }
                        return;
                    }
                }
            }
            else if (currentModal == MenuModal::Saves)
            {
                bool handled = false;
                for (int i = 0; i < 3; i++)
                {
                    const SaveSlotInfo& sInfo = (i < static_cast<int>(cachedSaveSlots.size())) ? cachedSaveSlots[i] : SaveSlotInfo{};

                    if (sInfo.exists)
                    {
                        // Кнопка [Загрузить / Записать]
                        if (UI::getSavesActionBtnRect(i).contains(static_cast<sf::Vector2f>(mpos)))
                        {
                            if (isSaveMode)
                            {
                                saveCurrentGame(i + 1);
                                refreshSaveSlots();
                            }
                            else
                            {
                                loadGameFromSlot(i + 1);
                            }
                            handled = true;
                            break;
                        }

                        // Кнопка [Удалить]
                        if (UI::getSavesDeleteBtnRect(i).contains(static_cast<sf::Vector2f>(mpos)))
                        {
                            if (pendingDeleteSlot == (i + 1))
                            {
                                deleteGameSlot(i + 1);
                                refreshSaveSlots();
                                pendingDeleteSlot = -1;
                            }
                            else
                            {
                                pendingDeleteSlot = i + 1;
                                sound.playClick();
                            }
                            handled = true;
                            break;
                        }
                    }
                    else if (isSaveMode)
                    {
                        // Пустой слот: кнопка [Сохранить]
                        if (UI::getSavesEmptyBtnRect(i).contains(static_cast<sf::Vector2f>(mpos)))
                        {
                            saveCurrentGame(i + 1);
                            refreshSaveSlots();
                            handled = true;
                            break;
                        }
                    }
                }

                // Кнопка [ НАЗАД ]
                if (!handled && UI::getSavesBackButtonRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    currentModal = MenuModal::None;
                    pendingDeleteSlot = -1;
                    sound.playClick();
                }
                else if (!handled)
                {
                    pendingDeleteSlot = -1;
                }
            }
            else if (currentModal == MenuModal::Settings)
            {
                // Кнопка звука [ - ]
                if (UI::getSettingsSoundMinusRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    settings.soundVolume = std::max(0, settings.soundVolume - 10);
                }
                // Кнопка звука [ + ]
                else if (UI::getSettingsSoundPlusRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    settings.soundVolume = std::min(100, settings.soundVolume + 10);
                }
                // Кнопка музыки [ - ]
                else if (UI::getSettingsMusicMinusRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    settings.musicVolume = std::max(0, settings.musicVolume - 10);
                }
                // Кнопка музыки [ + ]
                else if (UI::getSettingsMusicPlusRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    settings.musicVolume = std::min(100, settings.musicVolume + 10);
                }
                // Кнопка Назад
                else if (UI::getSettingsBackButtonRect().contains(static_cast<sf::Vector2f>(mpos)))
                {
                    currentModal = MenuModal::None;
                }
            }
        }
    }
    else if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            if (currentModal != MenuModal::None)
            {
                currentModal = MenuModal::None;
            }
            else
            {
                gameState = GameState::Playing;
            }
        }
    }
}

// ============================================
// СИСТЕМА СОХРАНЕНИЙ И СЛОТОВ
// ============================================
void Game::refreshSaveSlots()
{
    cachedSaveSlots = SaveManager::getAllSlotsInfo();
}

void Game::saveCurrentGame(int slot)
{
    currentSaveSlot = slot;
    bool ok = SaveManager::saveGame(slot, player, inv, timeSystem, world, house, chests, furniture, alchemy);
    if (ok)
    {
        sound.playCoin();
        FloatingText ft;
        ft.text = "Игра сохранена в Слот " + std::to_string(slot) + "!";
        ft.position = { 340.f, 210.f };
        ft.color = sf::Color(140, 255, 140);
        ft.timer = 2.5f;
        floatingTexts.push_back(ft);
    }
}

void Game::loadGameFromSlot(int slot)
{
    if (!SaveManager::doesSlotExist(slot)) return;
    currentSaveSlot = slot;
    currentModal = MenuModal::None;
    sound.playClick();

    gameState = GameState::Loading;
    loadingProgress = 0.f;
    loadingTimer = 0.f;
    currentTipIndex = rand() % 5;
    loadingStageText = "Загрузка сохраненных данных мира...";

    SaveManager::loadGame(slot, player, inv, timeSystem, world, house, chests, furniture, alchemy);
    triggerActiveSlotNotification();
}

void Game::deleteGameSlot(int slot)
{
    SaveManager::deleteSlot(slot);
    sound.playPop();
    FloatingText ft;
    ft.text = "Сохранение в слоте " + std::to_string(slot) + " удалено.";
    ft.position = { 340.f, 210.f };
    ft.color = sf::Color(255, 130, 130);
    ft.timer = 2.0f;
    floatingTexts.push_back(ft);
}

// ============================================
// СОН В КРОВАТИ И СМЕНА ДНЯ
// ============================================
void Game::sleepInBed(bool fullRest)
{
    if (isTransitioningSleep || isTransitioningWake) return;
    isTransitioningSleep = true;
    fadeTarget = 255.f;
    sleepReport.fullRest = fullRest;
    sound.playSleep();
}

void Game::triggerSleep(bool fullRest)
{
    sleepReport.fullRest = fullRest;
    sleepReport.dayNumber = timeSystem.currentDay;
    sleepReport.soldItems.clear();
    sleepReport.totalEarned = 0;
    sleepReport.previousGold = player.gold;

    // Ищем ящик сбыта (Shipping Bin)
    Chest* shippingBin = nullptr;
    for (auto& c : chests)
    {
        if (c.type == "shipping_bin")
        {
            shippingBin = &c;
            break;
        }
    }

    if (shippingBin != nullptr)
    {
        std::map<std::string, int> aggregated;
        for (int i = 0; i < shippingBin->slotCount; i++)
        {
            const Item& itm = shippingBin->inventory.getSlot(i);
            if (!itm.isEmpty())
            {
                aggregated[itm.id] += itm.count;
            }
        }

        for (const auto& [id, count] : aggregated)
        {
            const ItemData* d = itemDB.find(id);
            int priceEach = d ? d->sellPrice : 0;
            int total = priceEach * count;

            SoldItemRecord rec;
            rec.id = id;
            rec.name = d ? d->name : id;
            rec.count = count;
            rec.priceEach = priceEach;
            rec.totalPrice = total;
            sleepReport.soldItems.push_back(rec);
            sleepReport.totalEarned += total;
        }

        // Очищаем ящик сбыта после ночной продажи
        shippingBin->inventory = Inventory();
        shippingBin->isOpen = false;
    }

    player.gold += sleepReport.totalEarned;
    sleepReport.currentGold = player.gold;

    // Закрываем любые окна
    isInventoryOpen = false;
    alchemy.isCauldronOpen = false;
    alchemy.isTableOpen = false;
    alchemy.showRecipes = false;
    if (nearbyChest) nearbyChest->isOpen = false;

    // Переходим в экран ночного сна и отчёта
    gameState = GameState::Sleeping;
    isSleepNextHovered = false;
}

void Game::wakeUpFromSleep()
{
    timeSystem.advanceDay(sleepReport.fullRest);
    world.updateGrowth();

    // Автоматический полив грядок в дождливый день
    if (timeSystem.isRaining)
    {
        for (int y = 0; y < World::HEIGHT; y++)
        {
            for (int x = 0; x < World::WIDTH; x++)
            {
                if (world.map[y][x].type == 1) // Вспаханная грядка
                {
                    world.map[y][x].watered = true;
                }
            }
        }
    }

    // Обновление ежедневных заказов жителей
    initDailyQuests(timeSystem.currentDay);

    house.isInside = true;
    player.shape.setPosition({ House::ROOM_X + 32.f + 16.f, House::ROOM_Y + 85.f + 10.f });

    if (sleepReport.fullRest)
    {
        player.energy = player.maxEnergy;
        player.isExhausted = false;

        FloatingText ft;
        ft.text = "Доброе утро! Новый день наступил (энергия 100%).";
        ft.position = { player.shape.getPosition().x - 120.f, player.shape.getPosition().y - 40.f };
        ft.color = sf::Color(140, 255, 160);
        ft.timer = 2.5f;
        floatingTexts.push_back(ft);
    }
    else
    {
        player.energy = player.maxEnergy * 0.5f;
        player.isExhausted = false;

        FloatingText ft;
        ft.text = "Вы очнулись в кровати после обморока (энергия 50%).";
        ft.position = { player.shape.getPosition().x - 130.f, player.shape.getPosition().y - 40.f };
        ft.color = sf::Color(255, 110, 110);
        ft.timer = 3.0f;
        floatingTexts.push_back(ft);
    }

    if (timeSystem.isRaining)
    {
        FloatingText rft;
        rft.text = "Сегодня идет дождь! Все грядки политы.";
        rft.position = { player.shape.getPosition().x - 90.f, player.shape.getPosition().y - 15.f };
        rft.color = sf::Color(140, 220, 255);
        rft.timer = 3.0f;
        floatingTexts.push_back(rft);
    }

    // Автоматическое сохранение прогресса в активный слот при пробуждении
    SaveManager::saveGame(currentSaveSlot, player, inv, timeSystem, world, house, chests, furniture, alchemy);
    refreshSaveSlots();

    gameState = GameState::Playing;
}

void Game::handleSleepingEvents(const sf::Event& event)
{
    if (isTransitioningSleep || isTransitioningWake) return;

    sf::Vector2i mpos = getMouseVirtualPixel();

    if (const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseBtn->button == sf::Mouse::Button::Left)
        {
            if (UI::getSleepReportNextButtonRect().contains(static_cast<sf::Vector2f>(mpos)))
            {
                sound.playCoin();
                isTransitioningWake = true;
                fadeTarget = 255.f;
                return;
            }
        }
    }
    else if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Space ||
            keyPressed->code == sf::Keyboard::Key::Enter ||
            keyPressed->code == sf::Keyboard::Key::E)
        {
            sound.playCoin();
            isTransitioningWake = true;
            fadeTarget = 255.f;
            return;
        }
    }
}

// ============================================
// ИГРОВОЙ ЦИКЛ
// ============================================
void Game::run()
{
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        handleEvents();
        update(dt);
        render();
    }
}

// ============================================
// ОБРАБОТКА ВВОДА
// ============================================
void Game::handleEvents()
{
    while (const auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            return;
        }

        // Автоматическое сохранение пропорций (letterbox / pillarbox) при изменении размера окна
        if (const auto* resized = event->getIf<sf::Event::Resized>())
        {
            updateLetterboxView(resized->size.x, resized->size.y);
            continue;
        }

        // Обработка главного меню
        if (gameState == GameState::MainMenu)
        {
            handleMenuEvents(*event);
            continue;
        }

        // Обработка экрана загрузки
        if (gameState == GameState::Loading)
        {
            handleLoadingEvents(*event);
            continue;
        }

        // Обработка меню паузы
        if (gameState == GameState::PauseMenu)
        {
            handlePauseEvents(*event);
            continue;
        }

        // Обработка сна и отчёта о доходах
        if (gameState == GameState::Sleeping)
        {
            handleSleepingEvents(*event);
            continue;
        }

        // Обработка доски объявлений и заказов
        if (isOrdersBoardOpen)
        {
            handleOrdersBoardEvents(*event);
            continue;
        }

        // Блокировка управления во время затемнения экрана при переходе сна
        if (isTransitioningSleep || isTransitioningWake)
        {
            continue;
        }

        // --- КЛАВИАТУРА ---
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            // Escape: закрыть открытые окна или открыть меню паузы
            if (keyPressed->code == sf::Keyboard::Key::Escape)
            {
                if (isOrdersBoardOpen) isOrdersBoardOpen = false;
                else if (alchemy.showRecipes) alchemy.showRecipes = false;
                else if (alchemy.isCauldronOpen) alchemy.isCauldronOpen = false;
                else if (alchemy.isTableOpen) alchemy.isTableOpen = false;
                else if (nearbyChest && nearbyChest->isOpen) nearbyChest->isOpen = false;
                else if (isInventoryOpen) isInventoryOpen = false;
                else
                {
                    gameState = GameState::PauseMenu;
                    hoveredPauseButton = -1;
                    currentModal = MenuModal::None;
                    refreshSaveSlots();
                }
                return;
            }

            // Быстрое сохранение на F5
            if (keyPressed->code == sf::Keyboard::Key::F5)
            {
                saveCurrentGame(currentSaveSlot);
                refreshSaveSlots();
                return;
            }

            // Открытие / закрытие рецептов в котле на R
            if (keyPressed->code == sf::Keyboard::Key::R)
            {
                if (alchemy.isCauldronOpen)
                {
                    alchemy.showRecipes = !alchemy.showRecipes;
                    return;
                }
            }

            // Открытие / закрытие инвентаря на I или Tab
            if (keyPressed->code == sf::Keyboard::Key::I || keyPressed->code == sf::Keyboard::Key::Tab)
            {
                if (!alchemy.isCauldronOpen && !alchemy.isTableOpen)
                {
                    isInventoryOpen = !isInventoryOpen;
                    if (isInventoryOpen && nearbyChest != nullptr)
                    {
                        nearbyChest->isOpen = false;
                    }
                }
                return;
            }

            // Выбор слота хотбара 1-9
            if (keyPressed->code >= sf::Keyboard::Key::Num1 && keyPressed->code <= sf::Keyboard::Key::Num9)
            {
                activeSlot = static_cast<int>(keyPressed->code) - static_cast<int>(sf::Keyboard::Key::Num1);
                triggerActiveSlotNotification();
                return;
            }

            // Взаимодействие на E
            if (keyPressed->code == sf::Keyboard::Key::E)
            {
                if (player.isActing) return;

                if (isOrdersBoardOpen)
                {
                    isOrdersBoardOpen = false;
                    return;
                }
                if (alchemy.isCauldronOpen)
                {
                    if (alchemy.showRecipes)
                    {
                        alchemy.showRecipes = false;
                        return;
                    }
                    alchemy.isCauldronOpen = false;
                    return;
                }
                if (alchemy.isTableOpen)
                {
                    alchemy.isTableOpen = false;
                    return;
                }
                if (nearbyChest != nullptr && nearbyChest->isOpen)
                {
                    nearbyChest->isOpen = false;
                    return;
                }
                if (isInventoryOpen)
                {
                    isInventoryOpen = false;
                    return;
                }

                // Открытие доски заказов снаружи
                if (nearOrdersBoard)
                {
                    isOrdersBoardOpen = true;
                    sound.playClick();
                    return;
                }

                sf::Vector2f ppos = player.shape.getPosition();
                const Item& activeItem = inv.getSlot(activeSlot);
                bool hasTool = (activeItem.id == "axe" || activeItem.id == "pickaxe");

                // 1. Двери (вход и выход из дома по направлению взгляда)
                if (house.isInside && house.isNearExitDoor(ppos) && player.dirY > 0)
                {
                    house.exit(player);
                    return;
                }
                if (!house.isInside && house.isNearExteriorDoor(ppos) && player.dirY < 0)
                {
                    house.enter(player);
                    return;
                }

                // 2. Если в руках топор или кирка: попытка забрать мебель
                if (hasTool)
                {
                    if (tryPickupFurniture())
                    {
                        player.playAction(activeItem.id == "axe" ? PlayerAction::Axe : PlayerAction::Pickaxe);
                        return;
                    }
                }

                // 3. Сундук
                if (nearbyChest != nullptr)
                {
                    nearbyChest->isOpen = !nearbyChest->isOpen;
                    return;
                }

                // 4. Мебель (кровать, котел, стол)
                if (nearbyFurnitureIdx >= 0 && nearbyFurnitureIdx < static_cast<int>(furniture.size()))
                {
                    auto& f = furniture[nearbyFurnitureIdx];
                    if (f.id == "bed")
                    {
                        sleepInBed(true);
                        return;
                    }
                    if (f.id == "cauldron")
                    {
                        alchemy.isCauldronOpen = !alchemy.isCauldronOpen;
                        return;
                    }
                    if (f.id == "alchemist_table")
                    {
                        alchemy.isTableOpen = !alchemy.isTableOpen;
                        return;
                    }
                }

                // 5. Установка мебели из рук перед собой (сундук, кровать, котел, стол, ковер)
                if (activeItem.id == "chest" || activeItem.id == "bed" || activeItem.id == "alchemist_table" ||
                    activeItem.id == "cauldron" || activeItem.id == "carpet")
                {
                    if (tryPlaceFurniture(activeItem.id))
                    {
                        return;
                    }
                }

                // 6. Набор воды во флакон
                if (activeItem.id == "empty_flask")
                {
                    int pcx = static_cast<int>((ppos.x + Player::SIZE / 2.f) / World::TILE_SIZE) + player.dirX;
                    int pcy = static_cast<int>((ppos.y + Player::SIZE / 2.f) / World::TILE_SIZE) + player.dirY;
                    if (pcx >= 0 && pcx < World::WIDTH && pcy >= 0 && pcy < World::HEIGHT)
                    {
                        if (world.map[pcy][pcx].type == 2) // Вода
                        {
                            inv.removeItem("empty_flask", 1);
                            inv.addItem("water_flask", 1, 99);
                            sound.playWater();
                            FloatingText ft;
                            ft.text = "+1 Флакон с чистой водой";
                            ft.position = { ppos.x - 20.f, ppos.y - 25.f };
                            ft.color = sf::Color(140, 220, 255);
                            ft.timer = 1.4f;
                            floatingTexts.push_back(ft);
                            return;
                        }
                    }
                }

                // 7. Анимация действий инструментами
                if (activeItem.id == "hoe") { player.playAction(PlayerAction::Hoe); player.useEnergy(2.f); sound.playHoe(); }
                else if (activeItem.id == "watering_can") { player.playAction(PlayerAction::Water); player.useEnergy(1.f); sound.playWater(); }
                else if (activeItem.id == "axe") { player.playAction(PlayerAction::Axe); player.useEnergy(2.f); sound.playAxe(); }
                else if (activeItem.id == "pickaxe") { player.playAction(PlayerAction::Pickaxe); player.useEnergy(2.f); sound.playPickaxe(); }

                // Взаимодействие с миром (грядки, сорняки, деревья)
                if (!house.isInside)
                {
                    world.interact(player, activeItem.id, inv, droppedItems);
                }
                return;
            }

            // Выбросить предмет на Q
            if (keyPressed->code == sf::Keyboard::Key::Q)
            {
                const Item& itm = inv.getSlot(activeSlot);
                if (!itm.isEmpty())
                {
                    DroppedItem drop;
                    drop.item = Item{ itm.id, 1 };
                    drop.position = {
                        player.shape.getPosition().x + player.dirX * 24.f,
                        player.shape.getPosition().y + player.dirY * 24.f
                    };
                    droppedItems.push_back(drop);
                    inv.removeItem(itm.id, 1);
                }
                return;
            }
        }

        // --- МЫШЬ (ЛКМ) ---
        if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mousePressed->button == sf::Mouse::Button::Left)
            {
                sf::Vector2i mousePixel = getMouseVirtualPixel();
                float mx = static_cast<float>(mousePixel.x);
                float my = static_cast<float>(mousePixel.y);

                // 1. Интерактивный котел
                if (alchemy.isCauldronOpen)
                {
                    float winX = 60.f;
                    float winY = 60.f;

                    // Если открыта книга рецептов: клик по [X] или вне закрывает её
                    if (alchemy.showRecipes)
                    {
                        float bx = winX + 110.f;
                        float by = winY + 35.f;
                        float cx = bx + 460.f - 36.f;
                        float cy = by + 12.f;
                        if (isInside(mx, my, cx, cy, 26.f) || !isInsideRect(mx, my, bx, by, 460.f, 410.f))
                        {
                            alchemy.showRecipes = false;
                            return;
                        }
                        return;
                    }

                    // Кнопка [X] закрытия котла
                    if (isInside(mx, my, winX + 735.f, winY + 20.f, 32.f))
                    {
                        alchemy.isCauldronOpen = false;
                        alchemy.showRecipes = false;
                        return;
                    }

                    // Кнопка "Рецепты [R]"
                    float recBtnX = winX + 510.f;
                    float recBtnY = winY + 16.f;
                    if (isInsideRect(mx, my, recBtnX, recBtnY, 145.f, 34.f))
                    {
                        alchemy.showRecipes = !alchemy.showRecipes;
                        return;
                    }

                    // Кнопка "Сварить"
                    float btnX = winX + 475.f;
                    float btnY = winY + 400.f;
                    float btnW = 160.f;
                    float btnH = 42.f;
                    if (isInsideRect(mx, my, btnX, btnY, btnW, btnH) && !alchemy.isBrewing)
                    {
                        std::string err;
                        if (!alchemy.tryStartBrewing(err))
                        {
                            FloatingText ft;
                            ft.text = err;
                            ft.position = { winX + 430.f, winY + 360.f };
                            ft.color = sf::Color(255, 120, 120);
                            ft.timer = 1.5f;
                            floatingTexts.push_back(ft);
                        }
                        else
                        {
                            sound.playBrew();
                        }
                        return;
                    }

                    // Центральный слот базы
                    float cauldronCenterX = winX + 555.f;
                    float cauldronCenterY = winY + 235.f;
                    float baseSlotX = cauldronCenterX - 20.f;
                    float baseSlotY = cauldronCenterY - 10.f;
                    if (isInside(mx, my, baseSlotX, baseSlotY, 40.f))
                    {
                        if (!alchemy.cauldronBase.isEmpty())
                        {
                            inv.addItem(alchemy.cauldronBase.id, alchemy.cauldronBase.count, 99);
                            alchemy.cauldronBase = Item{};
                            sound.playPop();
                        }
                        return;
                    }

                    // 6 слотов ингредиентов вокруг котла
                    float radius = 85.f;
                    for (int i = 0; i < 6; i++)
                    {
                        float angle = i * (3.14159265f / 3.f) - 3.14159265f / 2.f;
                        float sx = cauldronCenterX + std::cos(angle) * radius - 20.f;
                        float sy = cauldronCenterY + std::sin(angle) * radius - 20.f;

                        if (isInside(mx, my, sx, sy, 40.f))
                        {
                            if (!alchemy.cauldronIngredients[i].isEmpty())
                            {
                                inv.addItem(alchemy.cauldronIngredients[i].id, alchemy.cauldronIngredients[i].count, 99);
                                alchemy.cauldronIngredients[i] = Item{};
                                sound.playPop();
                            }
                            return;
                        }
                    }

                    // Слоты инвентаря слева
                    float invX = winX + 20.f;
                    float invY = winY + 80.f;
                    float slotSize = 40.f;
                    float slotPad = 4.f;

                    for (int row = 0; row < 4; row++)
                    {
                        for (int col = 0; col < 9; col++)
                        {
                            float sx = invX + col * (slotSize + slotPad);
                            float sy = invY + row * (slotSize + slotPad);
                            if (isInside(mx, my, sx, sy, slotSize))
                            {
                                int slotIdx = (row == 3) ? col : (9 + row * 9 + col);
                                const Item& item = inv.getSlot(slotIdx);
                                if (!item.isEmpty())
                                {
                                    if (item.id == "empty_flask" || item.id == "water_flask")
                                    {
                                        if (alchemy.cauldronBase.isEmpty())
                                        {
                                            alchemy.cauldronBase = Item{ item.id, 1 };
                                            inv.removeItem(item.id, 1);
                                            sound.playPop();
                                        }
                                    }
                                    else
                                    {
                                        bool added = false;
                                        for (int i = 0; i < 6; i++)
                                        {
                                            if (!alchemy.cauldronIngredients[i].isEmpty() &&
                                                alchemy.cauldronIngredients[i].id == item.id &&
                                                alchemy.cauldronIngredients[i].count < 99)
                                            {
                                                alchemy.cauldronIngredients[i].count++;
                                                inv.removeItem(item.id, 1);
                                                sound.playPop();
                                                added = true;
                                                break;
                                            }
                                        }
                                        if (!added)
                                        {
                                            for (int i = 0; i < 6; i++)
                                            {
                                                if (alchemy.cauldronIngredients[i].isEmpty())
                                                {
                                                    alchemy.cauldronIngredients[i] = Item{ item.id, 1 };
                                                    inv.removeItem(item.id, 1);
                                                    sound.playPop();
                                                    added = true;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                                return;
                            }
                        }
                    }
                    return;
                }

                // 2. Стол алхимика (Ступка и пестик)
                if (alchemy.isTableOpen)
                {
                    float winX = 60.f;
                    float winY = 60.f;

                    // Кнопка [X]
                    if (isInside(mx, my, winX + 735.f, winY + 20.f, 32.f))
                    {
                        alchemy.isTableOpen = false;
                        return;
                    }

                    float rightCenterX = winX + 600.f;
                    float inX = rightCenterX - 110.f;
                    float inY = winY + 180.f;
                    float outX = rightCenterX + 60.f;
                    float outY = winY + 180.f;

                    // Забрать из входного слота
                    if (isInside(mx, my, inX, inY, 50.f))
                    {
                        if (!alchemy.mortarInput.isEmpty())
                        {
                            inv.addItem(alchemy.mortarInput.id, alchemy.mortarInput.count, 99);
                            alchemy.mortarInput = Item{};
                        }
                        return;
                    }

                    // Забрать из выходного слота
                    if (isInside(mx, my, outX, outY, 50.f))
                    {
                        if (!alchemy.mortarOutput.isEmpty())
                        {
                            inv.addItem(alchemy.mortarOutput.id, alchemy.mortarOutput.count, 99);
                            alchemy.mortarOutput = Item{};
                        }
                        return;
                    }

                    // Кнопка "Перетереть"
                    float btnX = rightCenterX - 90.f;
                    float btnY = winY + 330.f;
                    float btnW = 180.f;
                    float btnH = 44.f;
                    if (isInsideRect(mx, my, btnX, btnY, btnW, btnH))
                    {
                        if (alchemy.isGrinding) return;

                        if (alchemy.mortarInput.isEmpty())
                        {
                            FloatingText ft;
                            ft.text = "Положите сырьё в ячейку слева!";
                            ft.position = { btnX - 10.f, btnY - 30.f };
                            ft.color = sf::Color(255, 120, 120);
                            ft.timer = 1.5f;
                            floatingTexts.push_back(ft);
                            return;
                        }

                        std::string resId = alchemy.getGrindResultId();
                        if (resId.empty())
                        {
                            FloatingText ft;
                            ft.text = "Этот предмет нельзя перетереть!";
                            ft.position = { btnX - 10.f, btnY - 30.f };
                            ft.color = sf::Color(255, 120, 120);
                            ft.timer = 1.5f;
                            floatingTexts.push_back(ft);
                            return;
                        }

                        if (!alchemy.mortarOutput.isEmpty())
                        {
                            if (alchemy.mortarOutput.id != resId)
                            {
                                FloatingText ft;
                                ft.text = "Заберите другой порошок из ячейки!";
                                ft.position = { btnX - 25.f, btnY - 30.f };
                                ft.color = sf::Color(255, 120, 120);
                                ft.timer = 1.8f;
                                floatingTexts.push_back(ft);
                                sound.playPop();
                                return;
                            }
                            if (alchemy.mortarOutput.count >= 99)
                            {
                                FloatingText ft;
                                ft.text = "Ячейка порошка переполнена!";
                                ft.position = { btnX - 10.f, btnY - 30.f };
                                ft.color = sf::Color(255, 120, 120);
                                ft.timer = 1.8f;
                                floatingTexts.push_back(ft);
                                return;
                            }
                        }

                        alchemy.startGrinding();
                        return;
                    }

                    // Слоты инвентаря слева
                    float invX = winX + 25.f;
                    float invY = winY + 90.f;
                    float slotSize = 40.f;
                    float slotPad = 4.f;

                    for (int row = 0; row < 4; row++)
                    {
                        for (int col = 0; col < 9; col++)
                        {
                            float sx = invX + col * (slotSize + slotPad);
                            float sy = invY + row * (slotSize + slotPad);
                            if (isInside(mx, my, sx, sy, slotSize))
                            {
                                int slotIdx = (row == 3) ? col : (9 + row * 9 + col);
                                const Item& item = inv.getSlot(slotIdx);
                                if (!item.isEmpty())
                                {
                                    if (item.id == "berry" || item.id == "leaves" || item.id == "fiber")
                                    {
                                        if (alchemy.mortarInput.isEmpty())
                                        {
                                            alchemy.mortarInput = Item{ item.id, 1 };
                                            inv.removeItem(item.id, 1);
                                        }
                                        else if (alchemy.mortarInput.id == item.id && alchemy.mortarInput.count < 99)
                                        {
                                            alchemy.mortarInput.count++;
                                            inv.removeItem(item.id, 1);
                                        }
                                    }
                                }
                                return;
                            }
                        }
                    }
                    return;
                }

                // 3. Вкладки в инвентаре
                bool chestOpen = (nearbyChest != nullptr && nearbyChest->isOpen);
                if (isInventoryOpen && !chestOpen)
                {
                    // Клик по вкладкам сверху (Рюкзак, Крафт, Дневник)
                    if (my >= 175.f && my <= 207.f && mx >= 225.f && mx <= 540.f)
                    {
                        int tabIdx = static_cast<int>((mx - 225.f) / 105.f);
                        if (tabIdx >= 0 && tabIdx < 3)
                        {
                            sound.playClick();
                            currentTab = static_cast<InventoryTab>(tabIdx);
                            return;
                        }
                    }

                    // Вкладка Дневника (выбор категорий справа)
                    if (currentTab == InventoryTab::Journal)
                    {
                        float tabX = 235.f + 350.f + 10.f; // 595.f
                        if (mx >= tabX && mx <= tabX + 120.f)
                        {
                            for (int c = 0; c < 3; c++)
                            {
                                float ty = 220.f + 15.f + c * 44.f;
                                if (my >= ty && my <= ty + 34.f)
                                {
                                    journalCategory = c;
                                    return;
                                }
                            }
                        }
                        return;
                    }

                    // Вкладка Крафта
                    if (currentTab == InventoryTab::Crafting)
                    {
                        for (int r = 0; r < 7; r++)
                        {
                            sf::FloatRect rect = UI::getCraftingRecipeRect(r);
                            if (rect.contains({ mx, my }))
                            {
                                sf::Vector2f ppos = player.shape.getPosition();
                                if (r == 0) // Сундук
                                {
                                    if (inv.countItem("wood") >= 20)
                                    {
                                        inv.removeItem("wood", 20);
                                        inv.addItem("chest", 1, 99);
                                        sound.playPop();
                                        FloatingText ft;
                                        ft.text = "+1 Сундук создан!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(255, 220, 100);
                                        ft.timer = 1.5f;
                                        floatingTexts.push_back(ft);
                                    }
                                    else
                                    {
                                        FloatingText ft;
                                        ft.text = "Нужно 20 Древесины!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(255, 100, 100);
                                        ft.timer = 1.3f;
                                        floatingTexts.push_back(ft);
                                    }
                                }
                                else if (r == 1) // Стол алхимика
                                {
                                    if (inv.countItem("wood") >= 15 && inv.countItem("stone") >= 10)
                                    {
                                        inv.removeItem("wood", 15);
                                        inv.removeItem("stone", 10);
                                        inv.addItem("alchemist_table", 1, 99);
                                        sound.playPop();
                                        FloatingText ft;
                                        ft.text = "+1 Стол алхимика создан!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(255, 215, 110);
                                        ft.timer = 1.5f;
                                        floatingTexts.push_back(ft);
                                    }
                                    else
                                    {
                                        FloatingText ft;
                                        ft.text = "Нужно 15 Дерева и 10 Камня!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(255, 100, 100);
                                        ft.timer = 1.3f;
                                        floatingTexts.push_back(ft);
                                    }
                                }
                                else if (r == 2) // Алхимический котел
                                {
                                    if (inv.countItem("stone") >= 15 && inv.countItem("wood") >= 5)
                                    {
                                        inv.removeItem("stone", 15);
                                        inv.removeItem("wood", 5);
                                        inv.addItem("cauldron", 1, 99);
                                        sound.playPop();
                                        FloatingText ft;
                                        ft.text = "+1 Алхимический котел создан!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(140, 255, 180);
                                        ft.timer = 1.5f;
                                        floatingTexts.push_back(ft);
                                    }
                                    else
                                    {
                                        FloatingText ft;
                                        ft.text = "Нужно 15 Камня и 5 Дерева!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(255, 100, 100);
                                        ft.timer = 1.3f;
                                        floatingTexts.push_back(ft);
                                    }
                                }
                                else if (r == 3) // Уютная кровать
                                {
                                    if (inv.countItem("wood") >= 25 && inv.countItem("fiber") >= 10)
                                    {
                                        inv.removeItem("wood", 25);
                                        inv.removeItem("fiber", 10);
                                        inv.addItem("bed", 1, 99);
                                        sound.playPop();
                                        FloatingText ft;
                                        ft.text = "+1 Кровать создана!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(255, 200, 140);
                                        ft.timer = 1.5f;
                                        floatingTexts.push_back(ft);
                                    }
                                    else
                                    {
                                        FloatingText ft;
                                        ft.text = "Нужно 25 Дерева и 10 Волокна!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(255, 100, 100);
                                        ft.timer = 1.3f;
                                        floatingTexts.push_back(ft);
                                    }
                                }
                                else if (r == 4) // Мягкий ковер
                                {
                                    if (inv.countItem("fiber") >= 12)
                                    {
                                        inv.removeItem("fiber", 12);
                                        inv.addItem("carpet", 1, 99);
                                        sound.playPop();
                                        FloatingText ft;
                                        ft.text = "+1 Ковер создан!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(200, 180, 255);
                                        ft.timer = 1.5f;
                                        floatingTexts.push_back(ft);
                                    }
                                    else
                                    {
                                        FloatingText ft;
                                        ft.text = "Нужно 12 Волокна!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(255, 100, 100);
                                        ft.timer = 1.3f;
                                        floatingTexts.push_back(ft);
                                    }
                                }
                                else if (r == 5) // Пустой флакон
                                {
                                    if (inv.countItem("stone") >= 1)
                                    {
                                        inv.removeItem("stone", 1);
                                        inv.addItem("empty_flask", 1, 99);
                                        sound.playPop();
                                        FloatingText ft;
                                        ft.text = "+1 Пустой флакон создан!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(180, 240, 255);
                                        ft.timer = 1.3f;
                                        floatingTexts.push_back(ft);
                                    }
                                    else
                                    {
                                        FloatingText ft;
                                        ft.text = "Нужен 1 Камень!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(255, 100, 100);
                                        ft.timer = 1.3f;
                                        floatingTexts.push_back(ft);
                                    }
                                }
                                else if (r == 6) // Семена
                                {
                                    if (inv.countItem("berry") >= 1)
                                    {
                                        inv.removeItem("berry", 1);
                                        inv.addItem("seeds", 2, 99);
                                        sound.playPop();
                                        FloatingText ft;
                                        ft.text = "+2 Семени получено!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(160, 255, 160);
                                        ft.timer = 1.3f;
                                        floatingTexts.push_back(ft);
                                    }
                                    else
                                    {
                                        FloatingText ft;
                                        ft.text = "Нужна 1 Ягода!";
                                        ft.position = ppos;
                                        ft.color = sf::Color(255, 100, 100);
                                        ft.timer = 1.3f;
                                        floatingTexts.push_back(ft);
                                    }
                                }
                                return;
                            }
                        }
                        return;
                    }
                }

                // 4. Перетаскивание предметов (Drag & Drop) и быстрый перенос (Shift + LMB)
                if (isInventoryOpen || chestOpen)
                {
                    bool isShift = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
                                   sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

                    if (chestOpen && nearbyChest != nullptr)
                    {
                        int cslot = getChestSlotUnderMouse(mousePixel);
                        if (cslot != -1)
                        {
                            if (isShift)
                            {
                                transferToPlayer(*nearbyChest, cslot, inv, itemDB);
                                return;
                            }
                            const Item& itm = nearbyChest->inventory.getSlot(cslot);
                            if (!itm.isEmpty())
                            {
                                isDragging = true;
                                draggedItem = itm;
                                draggedFromSlot = cslot;
                                draggedFromChest = true;
                                nearbyChest->inventory.setSlot(cslot, Item{});
                                return;
                            }
                        }
                    }

                    int slot = getSlotUnderMouse(mousePixel);
                    if (slot != -1)
                    {
                        if (chestOpen && nearbyChest != nullptr && isShift)
                        {
                            transferToChest(inv, slot, *nearbyChest, itemDB);
                            return;
                        }
                        const Item& itm = inv.getSlot(slot);
                        if (!itm.isEmpty())
                        {
                            isDragging = true;
                            draggedItem = itm;
                            draggedFromSlot = slot;
                            draggedFromChest = false;
                            draggedFromHotbar = (slot < 9);
                            inv.setSlot(slot, Item{});
                            return;
                        }
                    }
                }
                else if (!alchemy.isCauldronOpen && !alchemy.isTableOpen)
                {
                    // Клик мыши по ячейке хотбара делает её активной (всегда в виртуальных координатах)
                    float totalHotbarWidth = UI::HOTBAR_SLOTS * UI::SLOT_SIZE + (UI::HOTBAR_SLOTS + 1) * UI::SLOT_PADDING;
                    float hotbarX = (VIRTUAL_WIDTH - totalHotbarWidth) / 2.f;
                    float hotbarY = VIRTUAL_HEIGHT - UI::SLOT_SIZE - 20.f;

                    bool clickedHotbar = false;
                    for (int i = 0; i < UI::HOTBAR_SLOTS; i++)
                    {
                        float sx = hotbarX + UI::SLOT_PADDING + i * (UI::SLOT_SIZE + UI::SLOT_PADDING);
                        if (isInside(mx, my, sx, hotbarY, UI::SLOT_SIZE))
                        {
                            activeSlot = i;
                            triggerActiveSlotNotification();
                            clickedHotbar = true;
                            return;
                        }
                    }

                    if (!clickedHotbar)
                    {
                        const Item& actItm = inv.getSlot(activeSlot);
                        if (actItm.id == "axe" || actItm.id == "pickaxe")
                        {
                            if (tryPickupFurniture())
                            {
                                player.playAction(actItm.id == "axe" ? PlayerAction::Axe : PlayerAction::Pickaxe);
                                return;
                            }
                        }
                        else if (actItm.id == "chest" || actItm.id == "bed" || actItm.id == "alchemist_table" ||
                                 actItm.id == "cauldron" || actItm.id == "carpet")
                        {
                            sf::Vector2f ppos = player.shape.getPosition();
                            int ptx = static_cast<int>((ppos.x + Player::SIZE / 2.f) / 32.f);
                            int pty = static_cast<int>((ppos.y + Player::SIZE / 2.f) / 32.f);
                            int htx = static_cast<int>(mx / 32.f);
                            int hty = static_cast<int>(my / 32.f);

                            if (std::abs(htx - ptx) <= 3 && std::abs(hty - pty) <= 3)
                            {
                                if (tryPlaceFurnitureAt(actItm.id, htx, hty))
                                {
                                    return;
                                }
                            }
                            else
                            {
                                if (tryPlaceFurniture(actItm.id))
                                {
                                    return;
                                }
                            }
                        }
                    }
                }
            }
            // ПКМ: Подробный осмотр предмета или употребление зелий/ягод
            else if (mousePressed->button == sf::Mouse::Button::Right)
            {
                sf::Vector2i mousePixel = getMouseVirtualPixel();
                float mx = static_cast<float>(mousePixel.x);
                float my = static_cast<float>(mousePixel.y);

                // 1. Проверяем клик ПКМ по предмету в котле
                if (alchemy.isCauldronOpen)
                {
                    float winX = 60.f;
                    float winY = 60.f;
                    float invX = winX + 20.f;
                    float invY = winY + 80.f;
                    float slotSize = 40.f;
                    float slotPad = 4.f;

                    for (int row = 0; row < 4; row++)
                    {
                        for (int col = 0; col < 9; col++)
                        {
                            float sx = invX + col * (slotSize + slotPad);
                            float sy = invY + row * (slotSize + slotPad);
                            if (isInside(mx, my, sx, sy, slotSize))
                            {
                                int slotIdx = (row == 3) ? col : (9 + row * 9 + col);
                                const Item& sItem = inv.getSlot(slotIdx);
                                if (!sItem.isEmpty()) { inspectedItemId = sItem.id; return; }
                            }
                        }
                    }

                    float cauldronCenterX = winX + 555.f;
                    float cauldronCenterY = winY + 235.f;
                    if (isInside(mx, my, cauldronCenterX - 20.f, cauldronCenterY - 10.f, 40.f) && !alchemy.cauldronBase.isEmpty())
                    {
                        inspectedItemId = alchemy.cauldronBase.id;
                        return;
                    }

                    float radius = 85.f;
                    for (int i = 0; i < 6; i++)
                    {
                        float angle = i * (3.14159265f / 3.f) - 3.14159265f / 2.f;
                        float sx = cauldronCenterX + std::cos(angle) * radius - 20.f;
                        float sy = cauldronCenterY + std::sin(angle) * radius - 20.f;
                        if (isInside(mx, my, sx, sy, 40.f) && !alchemy.cauldronIngredients[i].isEmpty())
                        {
                            inspectedItemId = alchemy.cauldronIngredients[i].id;
                            return;
                        }
                    }
                    return;
                }

                // 2. Проверяем клик ПКМ по предмету на столе алхимика
                if (alchemy.isTableOpen)
                {
                    float winX = 60.f;
                    float winY = 60.f;
                    float inX = winX + 600.f - 110.f;
                    float inY = winY + 180.f;
                    float outX = winX + 600.f + 60.f;
                    float outY = winY + 180.f;

                    if (isInside(mx, my, inX, inY, 50.f) && !alchemy.mortarInput.isEmpty())
                    {
                        inspectedItemId = alchemy.mortarInput.id;
                        return;
                    }
                    if (isInside(mx, my, outX, outY, 50.f) && !alchemy.mortarOutput.isEmpty())
                    {
                        inspectedItemId = alchemy.mortarOutput.id;
                        return;
                    }

                    float invX = winX + 25.f;
                    float invY = winY + 90.f;
                    float slotSize = 40.f;
                    float slotPad = 4.f;
                    for (int row = 0; row < 4; row++)
                    {
                        for (int col = 0; col < 9; col++)
                        {
                            float sx = invX + col * (slotSize + slotPad);
                            float sy = invY + row * (slotSize + slotPad);
                            if (isInside(mx, my, sx, sy, slotSize))
                            {
                                int slotIdx = (row == 3) ? col : (9 + row * 9 + col);
                                const Item& sItem = inv.getSlot(slotIdx);
                                if (!sItem.isEmpty()) { inspectedItemId = sItem.id; return; }
                            }
                        }
                    }
                    return;
                }

                // 3. Проверяем клик ПКМ по предмету в инвентаре или сундуке
                if (isInventoryOpen || (nearbyChest && nearbyChest->isOpen))
                {
                    if (nearbyChest && nearbyChest->isOpen)
                    {
                        int cslot = getChestSlotUnderMouse(mousePixel);
                        if (cslot != -1 && !nearbyChest->inventory.getSlot(cslot).isEmpty())
                        {
                            inspectedItemId = nearbyChest->inventory.getSlot(cslot).id;
                            return;
                        }
                    }
                    int slot = getSlotUnderMouse(mousePixel);
                    if (slot != -1 && !inv.getSlot(slot).isEmpty())
                    {
                        inspectedItemId = inv.getSlot(slot).id;
                        return;
                    }
                    return;
                }

                // 4. Проверяем клик ПКМ по ячейке хотбара на главном экране (в виртуальных координатах)
                float totalHotbarWidth = UI::HOTBAR_SLOTS * UI::SLOT_SIZE + (UI::HOTBAR_SLOTS + 1) * UI::SLOT_PADDING;
                float hotbarX = (VIRTUAL_WIDTH - totalHotbarWidth) / 2.f;
                float hotbarY = VIRTUAL_HEIGHT - UI::SLOT_SIZE - 20.f;

                for (int i = 0; i < UI::HOTBAR_SLOTS; i++)
                {
                    float sx = hotbarX + UI::SLOT_PADDING + i * (UI::SLOT_SIZE + UI::SLOT_PADDING);
                    if (isInside(mx, my, sx, hotbarY, UI::SLOT_SIZE))
                    {
                        const Item& sItem = inv.getSlot(i);
                        if (!sItem.isEmpty())
                        {
                            inspectedItemId = sItem.id;
                            return;
                        }
                    }
                }

                // 5. Если клик ПКМ совершен в мире — употребление зелья/ягоды
                const Item& itm = inv.getSlot(activeSlot);
                if (!itm.isEmpty())
                {
                    sf::Vector2f ppos = player.shape.getPosition();
                    if (itm.id == "energy_potion")
                    {
                        inv.removeItem("energy_potion", 1);
                        inv.addItem("empty_flask", 1, 99);
                        player.energy = std::min(player.maxEnergy, player.energy + 50.f);
                        player.isExhausted = false;

                        FloatingText ft;
                        ft.text = "+50 Энергии!";
                        ft.position = ppos;
                        ft.color = sf::Color(120, 255, 140);
                        ft.timer = 1.5f;
                        floatingTexts.push_back(ft);
                    }
                    else if (itm.id == "speed_potion")
                    {
                        inv.removeItem("speed_potion", 1);
                        inv.addItem("empty_flask", 1, 99);
                        player.speedBuffTimer = 30.f;

                        FloatingText ft;
                        ft.text = "+40% Скорости (30 сек)!";
                        ft.position = ppos;
                        ft.color = sf::Color(255, 230, 90);
                        ft.timer = 1.5f;
                        floatingTexts.push_back(ft);
                    }
                    else if (itm.id == "growth_potion")
                    {
                        inv.removeItem("growth_potion", 1);
                        inv.addItem("empty_flask", 1, 99);

                        // Ускоряем грядки вокруг игрока
                        int pcx = static_cast<int>((ppos.x + Player::SIZE / 2.f) / World::TILE_SIZE);
                        int pcy = static_cast<int>((ppos.y + Player::SIZE / 2.f) / World::TILE_SIZE);
                        for (int dy = -2; dy <= 2; dy++)
                        {
                            for (int dx = -2; dx <= 2; dx++)
                            {
                                int tx = pcx + dx;
                                int ty = pcy + dy;
                                if (tx >= 0 && tx < World::WIDTH && ty >= 0 && ty < World::HEIGHT)
                                {
                                    if (world.map[ty][tx].plantStage > 0 && world.map[ty][tx].plantStage < 4)
                                    {
                                        world.map[ty][tx].plantStage = 4; // Мгновенный урожай
                                    }
                                }
                            }
                        }

                        FloatingText ft;
                        ft.text = "Эликсир роста: Посевы созрели!";
                        ft.position = ppos;
                        ft.color = sf::Color(230, 160, 255);
                        ft.timer = 1.8f;
                        floatingTexts.push_back(ft);
                    }
                    else if (itm.id == "berry")
                    {
                        inv.removeItem("berry", 1);
                        player.energy = std::min(player.maxEnergy, player.energy + 15.f);
                        player.isExhausted = false;

                        FloatingText ft;
                        ft.text = "+15 Энергии";
                        ft.position = ppos;
                        ft.color = sf::Color(255, 180, 200);
                        ft.timer = 1.2f;
                        floatingTexts.push_back(ft);
                    }
                    else if (itm.id == "chest" || itm.id == "bed" || itm.id == "alchemist_table" ||
                             itm.id == "cauldron" || itm.id == "carpet" || itm.id == "window")
                    {
                        tryPlaceFurniture(itm.id);
                    }
                }
            }
        }

        // --- МЫШЬ (ОТПУСКАНИЕ КНОПОК) ---
        if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseReleased->button == sf::Mouse::Button::Right)
            {
                inspectedItemId.clear();
            }
            if (mouseReleased->button == sf::Mouse::Button::Left && isDragging)
            {
                sf::Vector2i mousePixel = getMouseVirtualPixel();
                float mx = static_cast<float>(mousePixel.x);
                float my = static_cast<float>(mousePixel.y);
                bool chestOpen = (nearbyChest != nullptr && nearbyChest->isOpen);

                // 1. Проверяем корзину (удаление предмета в режиме инвентаря)
                if (!chestOpen && isInside(mx, my, 665.f, 220.f + 3 * (UI::SLOT_SIZE + UI::SLOT_PADDING) + 10.f + UI::SLOT_SIZE + 10.f, UI::SLOT_SIZE))
                {
                    FloatingText ft;
                    ft.text = "Предмет удален";
                    ft.position = player.shape.getPosition();
                    ft.color = sf::Color(255, 120, 120);
                    ft.timer = 1.0f;
                    floatingTexts.push_back(ft);

                    isDragging = false;
                    draggedItem = Item{};
                    draggedFromSlot = -1;
                    draggedFromChest = false;
                    return;
                }

                int targetChestSlot = chestOpen ? getChestSlotUnderMouse(mousePixel) : -1;
                int targetInvSlot = getSlotUnderMouse(mousePixel);

                if (targetChestSlot != -1 && nearbyChest != nullptr)
                {
                    // Дроп в слот сундука
                    const Item& existing = nearbyChest->inventory.getSlot(targetChestSlot);
                    if (draggedFromChest)
                    {
                        if (existing.isEmpty())
                        {
                            nearbyChest->inventory.setSlot(targetChestSlot, draggedItem);
                        }
                        else if (existing.id == draggedItem.id)
                        {
                            int newCount = existing.count + draggedItem.count;
                            nearbyChest->inventory.setSlot(targetChestSlot, Item{ existing.id, newCount });
                        }
                        else
                        {
                            nearbyChest->inventory.setSlot(draggedFromSlot, existing);
                            nearbyChest->inventory.setSlot(targetChestSlot, draggedItem);
                        }
                    }
                    else
                    {
                        // Из инвентаря в сундук
                        if (nearbyChest->type == "shipping_bin")
                        {
                            const ItemData* itmData = itemDB.find(draggedItem.id);
                            if (draggedItem.id == "hoe" || draggedItem.id == "watering_can" || draggedItem.id == "axe" || draggedItem.id == "pickaxe" ||
                                (itmData && (itmData->isTool || itmData->sellPrice <= 0)))
                            {
                                inv.setSlot(draggedFromSlot, draggedItem);
                                isDragging = false;
                                draggedItem = Item{};
                                draggedFromSlot = -1;
                                draggedFromChest = false;

                                FloatingText ft;
                                ft.text = "Инструменты нельзя продавать! Они нужны для работы.";
                                ft.position = player.shape.getPosition();
                                ft.color = sf::Color(255, 120, 120);
                                ft.timer = 1.8f;
                                floatingTexts.push_back(ft);
                                sound.playPop();
                                return;
                            }
                        }

                        if (existing.isEmpty())
                        {
                            nearbyChest->inventory.setSlot(targetChestSlot, draggedItem);
                        }
                        else if (existing.id == draggedItem.id)
                        {
                            int newCount = existing.count + draggedItem.count;
                            nearbyChest->inventory.setSlot(targetChestSlot, Item{ existing.id, newCount });
                        }
                        else
                        {
                            inv.setSlot(draggedFromSlot, existing);
                            nearbyChest->inventory.setSlot(targetChestSlot, draggedItem);
                        }
                    }
                }
                else if (targetInvSlot != -1)
                {
                    // Дроп в слот инвентаря игрока
                    const Item& existing = inv.getSlot(targetInvSlot);
                    if (draggedFromChest && nearbyChest != nullptr)
                    {
                        // Из сундука в инвентарь
                        if (existing.isEmpty())
                        {
                            inv.setSlot(targetInvSlot, draggedItem);
                        }
                        else if (existing.id == draggedItem.id)
                        {
                            int newCount = existing.count + draggedItem.count;
                            inv.setSlot(targetInvSlot, Item{ existing.id, newCount });
                        }
                        else
                        {
                            nearbyChest->inventory.setSlot(draggedFromSlot, existing);
                            inv.setSlot(targetInvSlot, draggedItem);
                        }
                    }
                    else
                    {
                        // Из инвентаря в инвентарь
                        if (existing.isEmpty())
                        {
                            inv.setSlot(targetInvSlot, draggedItem);
                        }
                        else if (existing.id == draggedItem.id)
                        {
                            int newCount = existing.count + draggedItem.count;
                            inv.setSlot(targetInvSlot, Item{ existing.id, newCount });
                        }
                        else
                        {
                            inv.setSlot(draggedFromSlot, existing);
                            inv.setSlot(targetInvSlot, draggedItem);
                        }
                    }
                }
                else
                {
                    // Возвращаем на исходное место
                    if (draggedFromChest && nearbyChest != nullptr)
                    {
                        nearbyChest->inventory.setSlot(draggedFromSlot, draggedItem);
                    }
                    else
                    {
                        inv.setSlot(draggedFromSlot, draggedItem);
                    }
                }

                isDragging = false;
                draggedItem = Item{};
                draggedFromSlot = -1;
                draggedFromChest = false;
            }
        }
    }
}

// ============================================
// ПОИСК СЛОТА СУНДУКА ПОД МЫШЬЮ
// ============================================
int Game::getChestSlotUnderMouse(sf::Vector2i mousePos)
{
    bool chestOpen = (nearbyChest != nullptr && nearbyChest->isOpen);
    if (!chestOpen || nearbyChest == nullptr) return -1;

    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    const float GRID_X = 235.f;
    const float GRID_Y = 165.f;
    int rows = nearbyChest->slotCount / 9;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float x = GRID_X + col * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            float y = GRID_Y + row * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            if (isInside(mx, my, x, y, UI::SLOT_SIZE))
            {
                return row * 9 + col;
            }
        }
    }
    return -1;
}

// ============================================
// ПОИСК СЛОТА ПОД МЫШЬЮ
// ============================================
int Game::getSlotUnderMouse(sf::Vector2i mousePos)
{
    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    const float GRID_X = 235.f;
    bool chestOpen = (nearbyChest != nullptr && nearbyChest->isOpen);
    float GRID_Y = chestOpen ? 340.f : 220.f;

    // Слоты 9-35 (рюкзак)
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float x = GRID_X + col * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            float y = GRID_Y + row * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            if (isInside(mx, my, x, y, UI::SLOT_SIZE))
            {
                return 9 + row * 9 + col;
            }
        }
    }

    // Хотбар 0-8
    float HOTBAR_Y = GRID_Y + 3 * (UI::SLOT_SIZE + UI::SLOT_PADDING) + 10.f;
    for (int col = 0; col < 9; col++)
    {
        float x = GRID_X + col * (UI::SLOT_SIZE + UI::SLOT_PADDING);
        if (isInside(mx, my, x, HOTBAR_Y, UI::SLOT_SIZE))
        {
            return col;
        }
    }

    return -1;
}

// ============================================
// УВЕДОМЛЕНИЕ О ПРЕДМЕТЕ В РУКЕ
// ============================================
void Game::triggerActiveSlotNotification()
{
    const Item& itm = inv.getSlot(activeSlot);
    if (!itm.isEmpty())
    {
        const ItemData* d = itemDB.find(itm.id);
        activeItemNotification = d ? d->name : itm.id;
        activeNotificationTimer = 1.5f;
    }
    else
    {
        activeItemNotification = "";
        activeNotificationTimer = 0.f;
    }
}

// ============================================
// ОБНОВЛЕНИЕ СОСТОЯНИЯ
// ============================================
void Game::update(float dt)
{
    // Синхронизация громкости музыки и аудио-системы
    sound.update();
    sound.setMusicVolume(static_cast<float>(settings.musicVolume));

    // Обновление плавного перехода экрана (Fade in / Fade out)
    if (fadeAlpha < fadeTarget)
    {
        fadeAlpha = std::min(fadeAlpha + fadeSpeed * dt, fadeTarget);
    }
    else if (fadeAlpha > fadeTarget)
    {
        fadeAlpha = std::max(fadeAlpha - fadeSpeed * dt, fadeTarget);
    }

    if (isTransitioningSleep && fadeAlpha >= 254.f)
    {
        isTransitioningSleep = false;
        triggerSleep(sleepReport.fullRest);
        fadeTarget = 0.f; // плавно осветляем экран отчёта о продажах
    }
    else if (isTransitioningWake && fadeAlpha >= 254.f)
    {
        isTransitioningWake = false;
        wakeUpFromSleep();
        fadeTarget = 0.f; // плавно осветляем экран пробуждения в новом дне
    }

    if (gameState == GameState::MainMenu)
    {
        updateMenu(dt);
        return;
    }

    if (gameState == GameState::Loading)
    {
        updateLoading(dt);
        return;
    }

    if (gameState == GameState::PauseMenu)
    {
        sf::Vector2i mpos = getMouseVirtualPixel();
        hoveredPauseButton = -1;
        for (int i = 0; i < 6; i++)
        {
            if (UI::getPauseMenuButtonRect(i).contains(static_cast<sf::Vector2f>(mpos)))
            {
                hoveredPauseButton = i;
                break;
            }
        }
        return;
    }

    if (gameState == GameState::Sleeping)
    {
        menuAnimTime += dt;
        sf::Vector2i mpos = getMouseVirtualPixel();
        isSleepNextHovered = UI::getSleepReportNextButtonRect().contains(static_cast<sf::Vector2f>(mpos));
        return;
    }

    // 1. Время и суточный цикл
    timeSystem.update(dt);

    // Комендантский час: если игрок не лег спать до 02:00 — потеря сознания!
    if (timeSystem.hasPassedOut())
    {
        sleepInBed(false);
    }

    // 2. Алхимия: варка в котле
    if (alchemy.isBrewing)
    {
        alchemy.brewTimer += dt;
        if (alchemy.brewTimer >= AlchemySystem::BREW_DURATION)
        {
            BrewResult res = alchemy.finishBrewing();
            sound.playBrew();
            FloatingText ft;
            ft.text = res.message;
            ft.position = { 500.f, 250.f };
            ft.color = res.success ? sf::Color(140, 255, 140) : sf::Color(255, 120, 120);
            ft.timer = 2.0f;
            floatingTexts.push_back(ft);
        }
    }

    // 3. Алхимия: измельчение в ступке
    if (alchemy.isGrinding)
    {
        alchemy.grindTimer += dt;
        if (alchemy.grindTimer >= AlchemySystem::GRIND_DURATION)
        {
            alchemy.finishGrinding();
            FloatingText ft;
            ft.text = "+1 Порошок получен!";
            ft.position = { 550.f, 250.f };
            ft.color = sf::Color(255, 215, 100);
            ft.timer = 1.3f;
            floatingTexts.push_back(ft);
        }
    }

    // 4. Обновление анимаций мебели (котел 4 кадра; стол алхимика статичный)
    for (auto& f : furniture)
    {
        if (f.id == "cauldron")
        {
            f.animTimer += dt;
            if (f.animTimer >= 0.18f)
            {
                f.animTimer -= 0.18f;
                f.animFrame = (f.animFrame + 1) % 4;
            }
        }
    }

    // Сброс осмотра предмета, если ПКМ не зажата
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
        inspectedItemId.clear();
    }

    // 5. Перемещение игрока
    if (!isInventoryOpen && !alchemy.isCauldronOpen && !alchemy.isTableOpen && !(nearbyChest && nearbyChest->isOpen) && !isTransitioningSleep && !isTransitioningWake)
    {
        std::vector<sf::FloatRect> obstacles;
        if (house.isInside)
        {
            obstacles = house.getInteriorObstacles();
            for (const auto& c : chests)
            {
                if (c.isInside)
                    obstacles.push_back(sf::FloatRect(c.position, { 32.f, 32.f }));
            }
            for (const auto& f : furniture)
            {
                if (f.isInside)
                {
                    auto obs = f.getObstacle();
                    if (obs.size.x > 0.f && obs.size.y > 0.f)
                        obstacles.push_back(obs);
                }
            }
        }
        else
        {
            obstacles = world.getObstacles();
            auto houseObs = house.getExteriorObstacles();
            obstacles.insert(obstacles.end(), houseObs.begin(), houseObs.end());
            for (const auto& c : chests)
            {
                if (!c.isInside)
                    obstacles.push_back(sf::FloatRect(c.position, { 32.f, 32.f }));
            }
            for (const auto& f : furniture)
            {
                if (!f.isInside)
                {
                    auto obs = f.getObstacle();
                    if (obs.size.x > 0.f && obs.size.y > 0.f)
                        obstacles.push_back(obs);
                }
            }
            // Доска объявлений снаружи (Tile 15, 10 -> { 480.f, 320.f }, ровно 1 тайл)
            obstacles.push_back(sf::FloatRect({ 480.f, 320.f }, { 32.f, 32.f }));
        }
        float winW = static_cast<float>(window.getSize().x);
        float winH = static_cast<float>(window.getSize().y);
        player.update(dt, winW, winH, obstacles);
    }

    // 6. Всплывающие уведомления и тексты
    if (activeNotificationTimer > 0.f)
    {
        activeNotificationTimer -= dt;
    }

    for (auto it = floatingTexts.begin(); it != floatingTexts.end(); )
    {
        it->timer -= dt;
        it->position.y -= 18.f * dt;
        if (it->timer <= 0.f)
        {
            it = floatingTexts.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // 7. Подбор предметов с земли на ферме
    if (!house.isInside)
    {
        sf::Vector2f ppos = player.shape.getPosition();
        for (auto it = droppedItems.begin(); it != droppedItems.end(); )
        {
            it->age += dt;
            float dx = ppos.x - it->position.x;
            float dy = ppos.y - it->position.y;
            if (dx * dx + dy * dy < 28.f * 28.f && it->age > 0.4f)
            {
                inv.addItem(it->item.id, it->item.count, 99);

                FloatingText ft;
                const ItemData* d = itemDB.find(it->item.id);
                ft.text = "+" + std::to_string(it->item.count) + " " + (d ? d->name : it->item.id);
                ft.position = it->position;
                ft.color = sf::Color(255, 255, 150);
                ft.timer = 1.2f;
                floatingTexts.push_back(ft);

                sound.playPop();
                it = droppedItems.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // 8. Проверка ближайших интерактивных объектов и подсказок
    Chest* prevNearby = nearbyChest;
    nearbyChest = nullptr;
    nearbyFurnitureIdx = -1;
    nearOrdersBoard = false;
    playerPrompt.clear();

    bool anyModalOpen = isInventoryOpen || (prevNearby != nullptr && prevNearby->isOpen) ||
                        alchemy.isCauldronOpen || alchemy.isTableOpen || isOrdersBoardOpen;
    if (anyModalOpen)
    {
        if (prevNearby != nullptr && prevNearby->isOpen)
        {
            nearbyChest = prevNearby;
        }
        return;
    }

    sf::Vector2f ppos = player.shape.getPosition();
    float pcx = ppos.x + Player::SIZE / 2.f;
    float pcy = ppos.y + Player::SIZE / 2.f;

    // Тайл строго перед персонажем (по направлению взгляда)
    int ptx = static_cast<int>(pcx / 32.f);
    int pty = static_cast<int>(pcy / 32.f);
    int targetTx = ptx + player.dirX;
    int targetTy = pty + player.dirY;
    sf::FloatRect targetTile({ targetTx * 32.f, targetTy * 32.f }, { 32.f, 32.f });

    const Item& activeItem = inv.getSlot(activeSlot);
    bool hasTool = (activeItem.id == "axe" || activeItem.id == "pickaxe");

    // 1. Проверка доски заказов снаружи (Tile 15, 10 -> { 480.f, 320.f })
    if (!house.isInside && targetTx == 15 && targetTy == 10)
    {
        nearOrdersBoard = true;
        playerPrompt = "[E] Доска объявлений и заказов";
    }

    // 2. Сундуки в текущей локации (взаимодействие ТОЛЬКО если персонаж смотрит прямо на тайл сундука)
    if (!nearOrdersBoard)
    {
        for (auto& chest : chests)
        {
            if (chest.isInside == house.isInside)
            {
                int cTx = static_cast<int>(std::round(chest.position.x / 32.f));
                int cTy = static_cast<int>(std::round(chest.position.y / 32.f));

                if (targetTx == cTx && targetTy == cTy)
                {
                    nearbyChest = &chest;
                    if (chest.type == "shipping_bin")
                    {
                        playerPrompt = "[E] Открыть ящик сбыта товаров";
                    }
                    else
                    {
                        if (hasTool) playerPrompt = "[E / ЛКМ] Забрать сундук";
                        else playerPrompt = "[E] Открыть сундук";
                    }
                    break;
                }
            }
        }
    }
    if (prevNearby != nullptr && nearbyChest != prevNearby)
    {
        prevNearby->isOpen = false;
    }

    // 3. Мебель в текущей локации (взаимодействие ТОЛЬКО если целевой тайл взгляда попадает на объект)
    if (nearbyChest == nullptr && !nearOrdersBoard)
    {
        for (size_t i = 0; i < furniture.size(); i++)
        {
            auto& f = furniture[i];
            if (f.isInside == house.isInside)
            {
                sf::FloatRect b = f.getBounds();
                if (b.findIntersection(targetTile).has_value())
                {
                    nearbyFurnitureIdx = static_cast<int>(i);
                    if (hasTool)
                    {
                        playerPrompt = "[E / ЛКМ] Забрать предмет";
                    }
                    else if (f.id == "bed")
                    {
                        playerPrompt = "[E] Лечь спать";
                    }
                    else if (f.id == "cauldron")
                    {
                        playerPrompt = "[E] Варить зелья";
                    }
                    else if (f.id == "alchemist_table")
                    {
                        playerPrompt = "[E] Измельчить ингредиенты";
                    }
                    break;
                }
            }
        }
    }

    // Двери и установка предметов (вход и выход только если смотрим в сторону двери)
    if (playerPrompt.empty())
    {
        if (house.isInside && house.isNearExitDoor(ppos) && player.dirY > 0)
        {
            playerPrompt = "[E] Выйти на ферму";
        }
        else if (!house.isInside && house.isNearExteriorDoor(ppos) && player.dirY < 0)
        {
            playerPrompt = "[E] Войти в дом";
        }
        else if (activeItem.id == "chest" || activeItem.id == "bed" || activeItem.id == "alchemist_table" ||
                 activeItem.id == "cauldron" || activeItem.id == "carpet")
        {
            playerPrompt = "[E] Установить перед собой | [ЛКМ] По сетке";
        }
    }
}

// ============================================
// ОТРИСОВКА
// ============================================
void Game::render()
{
    window.clear(sf::Color::Black);
    window.setView(letterboxView);

    if (gameState == GameState::MainMenu)
    {
        ui.drawMainMenu(window, menuAnimTime, hoveredMenuButton, currentModal, settings, cachedSaveSlots, isSaveMode, pendingDeleteSlot, menuParticles);
        window.display();
        return;
    }

    if (gameState == GameState::Loading)
    {
        window.clear(sf::Color::Black);
        window.setView(letterboxView);
        const std::vector<std::string> loadingTips = {
            "Зажмите ПКМ на любом предмете в инвентаре для просмотра подробного описания и свойств.",
            "Внутри дома можно свободно расставлять и переносить мебель с помощью сетки на 3 клетки.",
            "Нажмите [R] возле котла, чтобы открыть алхимическую книгу рецептов зелий.",
            "Зажимайте Shift + ЛКМ при открытом сундуке для быстрого перемещения предметов в сундук.",
            "Ложитесь спать до 02:00 ночи, чтобы утром полностью восстановить энергию (100%)."
        };
        std::string tip = loadingTips[currentTipIndex % loadingTips.size()];
        ui.drawLoadingScreen(window, loadingProgress, loadingStageText, tip, menuAnimTime);
        window.display();
        return;
    }

    if (gameState == GameState::Sleeping)
    {
        window.clear();
        ui.drawSleepReport(window, sleepReport, menuAnimTime, isSleepNextHovered);
        if (fadeAlpha > 0.5f)
        {
            sf::RectangleShape fadeRect({ VIRTUAL_WIDTH, VIRTUAL_HEIGHT });
            fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(std::clamp(fadeAlpha, 0.f, 255.f))));
            window.draw(fadeRect);
        }
        window.display();
        return;
    }

    window.clear(sf::Color(20, 20, 25));

    sf::Vector2f ppos = player.shape.getPosition();

    // 1. Отрисовка мира / интерьера дома (2 независимые локации)
    if (house.isInside)
    {
        // ЛОКАЦИЯ 2: ИНТЕРЬЕР ДОМА (ОТДЕЛЬНАЯ СЦЕНА)
        house.drawInterior(window);

        // Ковры на полу (под ногами игрока и под мебелью)
        for (const auto& f : furniture)
        {
            if (f.isInside && f.id == "carpet")
            {
                drawFurniturePiece(window, f);
            }
        }

        // Сундуки и мебель сзади игрока (выше по Y)
        for (const auto& c : chests)
        {
            if (c.isInside && c.position.y + 16.f <= ppos.y + 18.f)
            {
                chestSprite->setPosition(c.position);
                window.draw(*chestSprite);
            }
        }
        for (const auto& f : furniture)
        {
            if (f.isInside && f.id != "carpet" && f.getSortY() <= ppos.y + 18.f)
            {
                drawFurniturePiece(window, f);
            }
        }

        // Игрок и предмет в руках
        player.draw(window);
        ui.drawHeldItem(window, player, inv.getSlot(activeSlot));

        // Сундуки и мебель спереди игрока (ниже по Y)
        for (const auto& c : chests)
        {
            if (c.isInside && c.position.y + 16.f > ppos.y + 18.f)
            {
                chestSprite->setPosition(c.position);
                window.draw(*chestSprite);
            }
        }
        for (const auto& f : furniture)
        {
            if (f.isInside && f.id != "carpet" && f.getSortY() > ppos.y + 18.f)
            {
                drawFurniturePiece(window, f);
            }
        }
    }
    else
    {
        // ЛОКАЦИЯ 1: ВНЕШНИЙ МИР / УЛИЦА
        world.draw(window);

        auto drawNoticeBoard = [&](sf::RenderWindow& w) {
            float bx = 480.f;
            float by = 320.f;

            // Две деревянные стойки
            sf::RectangleShape postL({ 4.f, 30.f });
            postL.setPosition({ bx + 4.f, by + 2.f });
            postL.setFillColor(sf::Color(85, 48, 22));
            w.draw(postL);

            sf::RectangleShape postR({ 4.f, 30.f });
            postR.setPosition({ bx + 24.f, by + 2.f });
            postR.setFillColor(sf::Color(85, 48, 22));
            w.draw(postR);

            // Доска-щит
            sf::RectangleShape board({ 30.f, 20.f });
            board.setPosition({ bx + 1.f, by + 3.f });
            board.setFillColor(sf::Color(160, 105, 55));
            board.setOutlineColor(sf::Color(70, 38, 16));
            board.setOutlineThickness(1.5f);
            w.draw(board);

            // Горизонтальный шов
            sf::RectangleShape seam({ 28.f, 1.f });
            seam.setPosition({ bx + 2.f, by + 13.f });
            seam.setFillColor(sf::Color(115, 70, 32));
            w.draw(seam);

            // Верхний козырек
            sf::RectangleShape roof({ 32.f, 4.f });
            roof.setPosition({ bx, by });
            roof.setFillColor(sf::Color(110, 60, 24));
            roof.setOutlineColor(sf::Color(55, 28, 10));
            roof.setOutlineThickness(1.f);
            w.draw(roof);

            // Листок объявления 1
            sf::RectangleShape note1({ 9.f, 11.f });
            note1.setPosition({ bx + 4.f, by + 7.f });
            note1.setFillColor(sf::Color(245, 238, 210));
            note1.setOutlineColor(sf::Color(170, 150, 115));
            note1.setOutlineThickness(0.8f);
            w.draw(note1);

            sf::CircleShape pin1(1.2f);
            pin1.setPosition({ bx + 7.5f, by + 6.5f });
            pin1.setFillColor(sf::Color(220, 50, 50));
            w.draw(pin1);

            // Листок объявления 2
            sf::RectangleShape note2({ 10.f, 12.f });
            note2.setPosition({ bx + 16.f, by + 8.f });
            note2.setFillColor(sf::Color(240, 230, 195));
            note2.setOutlineColor(sf::Color(170, 150, 115));
            note2.setOutlineThickness(0.8f);
            w.draw(note2);

            sf::CircleShape pin2(1.2f);
            pin2.setPosition({ bx + 19.5f, by + 7.5f });
            pin2.setFillColor(sf::Color(50, 120, 220));
            w.draw(pin2);
        };

        // 1. Сундуки, доска и мебель сзади игрока (выше по Y)
        if (320.f + 16.f <= ppos.y + 18.f)
        {
            drawNoticeBoard(window);
        }
        for (const auto& chest : chests)
        {
            if (!chest.isInside && chest.position.y + 16.f <= ppos.y + 18.f)
            {
                if (chest.type == "shipping_bin")
                {
                    sf::RectangleShape binBase({ 32.f, 22.f });
                    binBase.setPosition({ chest.position.x, chest.position.y + 10.f });
                    binBase.setFillColor(sf::Color(135, 80, 42));
                    binBase.setOutlineColor(sf::Color(65, 38, 18));
                    binBase.setOutlineThickness(1.5f);
                    window.draw(binBase);

                    sf::RectangleShape plank({ 32.f, 2.f });
                    plank.setPosition({ chest.position.x, chest.position.y + 20.f });
                    plank.setFillColor(sf::Color(95, 55, 25));
                    window.draw(plank);

                    sf::RectangleShape binLid({ 34.f, 10.f });
                    binLid.setPosition({ chest.position.x - 1.f, chest.position.y + 2.f });
                    binLid.setFillColor(sf::Color(165, 105, 55));
                    binLid.setOutlineColor(sf::Color(80, 45, 20));
                    binLid.setOutlineThickness(1.5f);
                    window.draw(binLid);

                    sf::RectangleShape bandL({ 4.f, 29.f });
                    bandL.setPosition({ chest.position.x + 4.f, chest.position.y + 3.f });
                    bandL.setFillColor(sf::Color(215, 175, 75));
                    bandL.setOutlineColor(sf::Color(130, 95, 30));
                    bandL.setOutlineThickness(1.f);
                    window.draw(bandL);

                    sf::RectangleShape bandR({ 4.f, 29.f });
                    bandR.setPosition({ chest.position.x + 24.f, chest.position.y + 3.f });
                    bandR.setFillColor(sf::Color(215, 175, 75));
                    bandR.setOutlineColor(sf::Color(130, 95, 30));
                    bandR.setOutlineThickness(1.f);
                    window.draw(bandR);

                    sf::CircleShape badge(6.f);
                    badge.setPosition({ chest.position.x + 10.f, chest.position.y - 12.f });
                    badge.setFillColor(sf::Color(255, 215, 30));
                    badge.setOutlineColor(sf::Color(180, 130, 15));
                    badge.setOutlineThickness(1.5f);
                    window.draw(badge);

                    sf::Text gTxt(ui.font);
                    gTxt.setString("G");
                    gTxt.setCharacterSize(10);
                    gTxt.setFillColor(sf::Color(90, 50, 0));
                    gTxt.setPosition({ chest.position.x + 13.5f, chest.position.y - 14.f });
                    window.draw(gTxt);
                }
                else if (chestSprite)
                {
                    chestSprite->setPosition(chest.position);
                    window.draw(*chestSprite);
                }
            }
        }
        for (const auto& f : furniture)
        {
            if (!f.isInside && f.getSortY() <= ppos.y + 18.f)
            {
                drawFurniturePiece(window, f);
            }
        }

        // Физика и глубина (Y-sorting) дома и игрока
        if (player.shape.getPosition().y + Player::SIZE < house.exteriorPos.y - 40.f)
        {
            player.draw(window);
            ui.drawHeldItem(window, player, inv.getSlot(activeSlot));
            house.drawExterior(window);
        }
        else
        {
            house.drawExterior(window);
            player.draw(window);
            ui.drawHeldItem(window, player, inv.getSlot(activeSlot));
        }

        // 2. Сундуки и мебель спереди игрока (ниже по Y)
        for (const auto& chest : chests)
        {
            if (!chest.isInside && chest.position.y + 16.f > ppos.y + 18.f)
            {
                if (chest.type == "shipping_bin")
                {
                    sf::RectangleShape binBase({ 32.f, 22.f });
                    binBase.setPosition({ chest.position.x, chest.position.y + 10.f });
                    binBase.setFillColor(sf::Color(135, 80, 42));
                    binBase.setOutlineColor(sf::Color(65, 38, 18));
                    binBase.setOutlineThickness(1.5f);
                    window.draw(binBase);

                    sf::RectangleShape plank({ 32.f, 2.f });
                    plank.setPosition({ chest.position.x, chest.position.y + 20.f });
                    plank.setFillColor(sf::Color(95, 55, 25));
                    window.draw(plank);

                    sf::RectangleShape binLid({ 34.f, 10.f });
                    binLid.setPosition({ chest.position.x - 1.f, chest.position.y + 2.f });
                    binLid.setFillColor(sf::Color(165, 105, 55));
                    binLid.setOutlineColor(sf::Color(80, 45, 20));
                    binLid.setOutlineThickness(1.5f);
                    window.draw(binLid);

                    sf::RectangleShape bandL({ 4.f, 29.f });
                    bandL.setPosition({ chest.position.x + 4.f, chest.position.y + 3.f });
                    bandL.setFillColor(sf::Color(215, 175, 75));
                    bandL.setOutlineColor(sf::Color(130, 95, 30));
                    bandL.setOutlineThickness(1.f);
                    window.draw(bandL);

                    sf::RectangleShape bandR({ 4.f, 29.f });
                    bandR.setPosition({ chest.position.x + 24.f, chest.position.y + 3.f });
                    bandR.setFillColor(sf::Color(215, 175, 75));
                    bandR.setOutlineColor(sf::Color(130, 95, 30));
                    bandR.setOutlineThickness(1.f);
                    window.draw(bandR);

                    sf::CircleShape badge(6.f);
                    badge.setPosition({ chest.position.x + 10.f, chest.position.y - 12.f });
                    badge.setFillColor(sf::Color(255, 215, 30));
                    badge.setOutlineColor(sf::Color(180, 130, 15));
                    badge.setOutlineThickness(1.5f);
                    window.draw(badge);

                    sf::Text gTxt(ui.font);
                    gTxt.setString("G");
                    gTxt.setCharacterSize(10);
                    gTxt.setFillColor(sf::Color(90, 50, 0));
                    gTxt.setPosition({ chest.position.x + 13.5f, chest.position.y - 14.f });
                    window.draw(gTxt);
                }
                else if (chestSprite)
                {
                    chestSprite->setPosition(chest.position);
                    window.draw(*chestSprite);
                }
            }
        }
        for (const auto& f : furniture)
        {
            if (!f.isInside && f.getSortY() > ppos.y + 18.f)
            {
                drawFurniturePiece(window, f);
            }
        }
        if (320.f + 16.f > ppos.y + 18.f)
        {
            drawNoticeBoard(window);
        }

        // Деревья
        world.drawTrees(window, player);

        // Предметы на земле
        for (const auto& drop : droppedItems)
        {
            ui.drawDroppedItem(window, drop);
        }

        // Маркер целевого тайла
        int targetX = static_cast<int>((ppos.x + Player::SIZE / 2.f) / World::TILE_SIZE) + player.dirX;
        int targetY = static_cast<int>((ppos.y + Player::SIZE / 2.f) / World::TILE_SIZE) + player.dirY;
        if (targetX >= 0 && targetX < World::WIDTH && targetY >= 0 && targetY < World::HEIGHT)
        {
            targetMarker.setPosition({ targetX * World::TILE_SIZE, targetY * World::TILE_SIZE });
            window.draw(targetMarker);
        }
    }

    // Сетка и зона установки предметов при удержании мебели в руках
    bool anyModalOpen = isInventoryOpen || (nearbyChest != nullptr && nearbyChest->isOpen) ||
                        alchemy.isCauldronOpen || alchemy.isTableOpen || isOrdersBoardOpen;
    if (!anyModalOpen)
    {
        drawPlacementGrid(window);
    }

    // 2. Ночное освещение и источники света
    std::vector<std::pair<sf::Vector2f, float>> lights;
    // Фонарь игрока
    lights.push_back({ { ppos.x + Player::SIZE / 2.f, ppos.y + Player::SIZE / 2.f }, 95.f });

    if (!house.isInside)
    {
        // Свет из окон дома снаружи ночью
        lights.push_back({ { house.exteriorPos.x + 32.f, house.exteriorPos.y - 70.f }, 85.f });
    }
    timeSystem.drawNightOverlay(window, lights);

    // 4. Экран затемнения (сон / переход)
    timeSystem.drawFade(window);

    // 5. HUD: день, часы (красные в полночь), золото
    ui.drawHUD(window, timeSystem.currentDay, timeSystem.getTimeString(), timeSystem.isCurfew(), timeSystem.getHour(), player.gold);

    // 6. Шкала выносливости (энергия)
    ui.drawEnergyBar(window, player.energy, player.maxEnergy);

    // 7. Хотбар
    bool chestOpen = (nearbyChest != nullptr && nearbyChest->isOpen);
    if (!isInventoryOpen && !chestOpen && !alchemy.isCauldronOpen && !alchemy.isTableOpen && !isOrdersBoardOpen)
    {
        ui.drawHotbar(window, inv, activeSlot, itemDB);
    }

    // 8. Окно инвентаря
    ui.drawInventory(window, inv, activeSlot, isInventoryOpen || chestOpen, isDragging, chestOpen, itemDB, static_cast<int>(currentTab), journalCategory, &alchemy);

    // 9. Интерактивный котел
    if (alchemy.isCauldronOpen)
    {
        ui.drawInteractiveCauldron(window, inv, alchemy, itemDB);
    }

    // 10. Стол алхимика (Ступка и пестик)
    if (alchemy.isTableOpen)
    {
        ui.drawMortarStation(window, inv, alchemy, itemDB);
    }

    // 11. Окно сундука
    if (chestOpen)
    {
        std::string cTitle = (nearbyChest->type == "shipping_bin") ? "Ящик сбыта товаров (продажа во время сна)" : "Сундук";
        ui.drawChest(window, nearbyChest->inventory, nearbyChest->slotCount, true, itemDB, cTitle);
    }

    // 12. Доска объявлений и лавка припасов
    if (isOrdersBoardOpen)
    {
        ui.drawOrdersBoardModal(window, ordersBoardTab, dailyQuests, shopCatalog, inv, player.gold, itemDB, getMouseVirtualCoords());
    }

    // 12. Перетаскиваемый предмет
    if (isDragging)
    {
        sf::Vector2i mousePixel = getMouseVirtualPixel();
        ui.drawItemIcon(window,
            static_cast<float>(mousePixel.x) - UI::SLOT_SIZE / 2.f,
            static_cast<float>(mousePixel.y) - UI::SLOT_SIZE / 2.f,
            draggedItem);
    }

    // 13. Тексты и подсказки
    if (activeNotificationTimer > 0.f && !activeItemNotification.empty())
    {
        ui.drawActiveItemNotification(window, activeItemNotification, activeNotificationTimer);
    }
    ui.drawFloatingTexts(window, floatingTexts);

    // Подсказка взаимодействия над персонажем (скрывается, если открыто любое модальное окно)
    if (!playerPrompt.empty() && !anyModalOpen)
    {
        sf::Text pTxt(ui.font);
        pTxt.setString(toUtf8(playerPrompt));
        pTxt.setCharacterSize(14);
        sf::FloatRect b = pTxt.getLocalBounds();
        pTxt.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y + b.size.y });

        float tx = ppos.x + Player::SIZE / 2.f;
        float ty = ppos.y - 8.f;

        // Тонкая тень для читаемости
        pTxt.setFillColor(sf::Color(0, 0, 0, 190));
        pTxt.setPosition({ tx + 1.f, ty + 1.f });
        window.draw(pTxt);

        // Белый текст без фона
        pTxt.setFillColor(sf::Color(255, 255, 255));
        pTxt.setPosition({ tx, ty });
        window.draw(pTxt);
    }

    // 14. Тултип предмета под курсором
    if (!ui.hoveredItemName.empty())
    {
        ui.drawTooltip(window, getMouseVirtualPixel(), ui.hoveredItemName);
        ui.hoveredItemName = "";
    }

    // 15. Подробная карточка предмета при зажатии ПКМ
    if (!inspectedItemId.empty())
    {
        const ItemData* d = itemDB.find(inspectedItemId);
        if (d)
        {
            ui.drawDetailedItemCard(window, getMouseVirtualPixel(), *d);
        }
    }

    // 16. Меню паузы поверх игры
    if (gameState == GameState::PauseMenu)
    {
        ui.drawPauseMenu(window, hoveredPauseButton, currentModal, settings, cachedSaveSlots, isSaveMode, pendingDeleteSlot);
    }

    // 17. Плавное затемнение экрана (Fade transition)
    if (fadeAlpha > 0.5f)
    {
        sf::RectangleShape fadeRect({ VIRTUAL_WIDTH, VIRTUAL_HEIGHT });
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(std::clamp(fadeAlpha, 0.f, 255.f))));
        window.draw(fadeRect);
    }

    window.display();
}

// ============================================
// TRANSFER TO CHEST
// ============================================
void Game::transferToChest(Inventory& srcInv, int srcSlot, Chest& chest, const ItemDatabase& itemDB)
{
    Item src = srcInv.getSlot(srcSlot);
    if (src.isEmpty()) return;

    if (chest.type == "shipping_bin")
    {
        const ItemData* itmData = itemDB.find(src.id);
        if (src.id == "hoe" || src.id == "watering_can" || src.id == "axe" || src.id == "pickaxe" ||
            (itmData && (itmData->isTool || itmData->sellPrice <= 0)))
        {
            FloatingText ft;
            ft.text = "Инструменты нельзя продавать! Они нужны для работы.";
            ft.position = player.shape.getPosition();
            ft.color = sf::Color(255, 120, 120);
            ft.timer = 1.8f;
            floatingTexts.push_back(ft);
            return;
        }
    }

    int maxStack = 99;
    const ItemData* d = itemDB.find(src.id);
    if (d && d->maxStack > 0) maxStack = d->maxStack;

    // 1. Ищем существующие стаки
    for (int i = 0; i < chest.slotCount; i++)
    {
        Item& slot = chest.inventory.slots[i];
        if (slot.id == src.id && slot.count < maxStack)
        {
            int space = maxStack - slot.count;
            int toMove = std::min(src.count, space);
            slot.count += toMove;
            src.count -= toMove;
            if (src.count <= 0)
            {
                srcInv.setSlot(srcSlot, Item{});
                return;
            }
        }
    }
    // 2. Ищем пустые слоты
    for (int i = 0; i < chest.slotCount; i++)
    {
        Item& slot = chest.inventory.slots[i];
        if (slot.isEmpty())
        {
            slot = src;
            srcInv.setSlot(srcSlot, Item{});
            return;
        }
    }
    srcInv.setSlot(srcSlot, src);
}

// ============================================
// БЫСТРЫЙ ПЕРЕНОС (SHIFT + КЛИК ИЗ СУНДУКА В ИНВЕНТАРЬ)
// ============================================
void Game::transferToPlayer(Chest& chest, int chestSlot, Inventory& playerInv, const ItemDatabase& itemDB)
{
    Item src = chest.inventory.getSlot(chestSlot);
    if (src.isEmpty()) return;
    int maxStack = 99;
    const ItemData* d = itemDB.find(src.id);
    if (d && d->maxStack > 0) maxStack = d->maxStack;

    // 1. Ищем существующие стаки
    for (int i = 0; i < Inventory::SIZE; i++)
    {
        Item& slot = playerInv.slots[i];
        if (slot.id == src.id && slot.count < maxStack)
        {
            int space = maxStack - slot.count;
            int toMove = std::min(src.count, space);
            slot.count += toMove;
            src.count -= toMove;
            if (src.count <= 0)
            {
                chest.inventory.setSlot(chestSlot, Item{});
                return;
            }
        }
    }
    // 2. Ищем пустые слоты
    for (int i = 0; i < Inventory::SIZE; i++)
    {
        Item& slot = playerInv.slots[i];
        if (slot.isEmpty())
        {
            slot = src;
            chest.inventory.setSlot(chestSlot, Item{});
            return;
        }
    }
    chest.inventory.setSlot(chestSlot, src);
}
