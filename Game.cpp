#include "Game.hpp"
#include <iostream>

// ============================================
// ВСПОМОГАТЕЛЬНОЕ
// ============================================
static bool isInside(float mx, float my, float x, float y, float size)
{
    return mx >= x && mx < x + size && my >= y && my < y + size;
}

// ============================================
// КОНСТРУКТОР
// ============================================
Game::Game()
    : window(sf::VideoMode({ 800, 600 }), "Alchemy Farm")
{
    // Загружаем базу предметов
    if (!itemDB.loadFromFile("data/items.json"))
    {
        std::cerr << "GAME: items.json NOT LOADED" << std::endl;
    }
    // Начальный инвентарь
    inv.addItem("hoe", 1, 1);
    inv.addItem("watering_can", 1, 1);
    inv.addItem("seeds", 5, 99);

    // Маркер тайла перед игроком
    targetMarker.setSize({ World::TILE_SIZE, World::TILE_SIZE });
    targetMarker.setFillColor(sf::Color(255, 255, 0, 100));
}

// ============================================
// ИГРОВОЙ ЦИКЛ
// ============================================
void Game::run()
{
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        handleEvents();
        update(dt);
        render();
    }
}

// ============================================
// ОБРАБОТКА СОБЫТИЙ
// ============================================
void Game::handleEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            // Хотбар: клавиши 1-9 выбирают слот 0-8
            if (keyPressed->code == sf::Keyboard::Key::Num1) activeSlot = 0;
            else if (keyPressed->code == sf::Keyboard::Key::Num2) activeSlot = 1;
            else if (keyPressed->code == sf::Keyboard::Key::Num3) activeSlot = 2;
            else if (keyPressed->code == sf::Keyboard::Key::Num4) activeSlot = 3;
            else if (keyPressed->code == sf::Keyboard::Key::Num5) activeSlot = 4;
            else if (keyPressed->code == sf::Keyboard::Key::Num6) activeSlot = 5;
            else if (keyPressed->code == sf::Keyboard::Key::Num7) activeSlot = 6;
            else if (keyPressed->code == sf::Keyboard::Key::Num8) activeSlot = 7;
            else if (keyPressed->code == sf::Keyboard::Key::Num9) activeSlot = 8;
            // Открыть/закрыть инвентарь
            if (keyPressed->code == sf::Keyboard::Key::I)
            {
                isInventoryOpen = !isInventoryOpen;
            }
            // Взаимодействие: берём id предмета из активного слота
            if (keyPressed->code == sf::Keyboard::Key::E)
            {
                const Item& activeItem = inv.getSlot(activeSlot);
                world.interact(player, activeItem.id, inv);
            }
        }
        // ============================================
        // МЫШЬ: НАЧАЛО ПЕРЕТАСКИВАНИЯ
        // ============================================
        if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mousePressed->button == sf::Mouse::Button::Left)
            {
                int slot = getSlotUnderMouse(sf::Mouse::getPosition(window));
                if (slot >= 0)
                {
                    const Item& item = inv.getSlot(slot);
                    if (!item.isEmpty())
                    {
                        isDragging = true;
                        draggedItem = item;
                        draggedFromSlot = slot;
                        inv.setSlot(slot, Item{});
                    }
                }
            }
        }

        // ============================================
        // МЫШЬ: ОТПУСКАНИЕ
        // ============================================
        if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseReleased->button == sf::Mouse::Button::Left && isDragging)
            {
                int slot = getSlotUnderMouse(sf::Mouse::getPosition(window));

                if (slot == -2)
                {
                    // Корзина — уничтожить предмет (ничего не делаем,
                    // draggedItem просто пропадёт)
                }
                else if (slot >= 0)
                {
                    Item& target = inv.slots[slot];
                    if (target.isEmpty())
                    {
                        inv.setSlot(slot, draggedItem);
                    }
                    else if (target.id == draggedItem.id)
                    {
                        target.count += draggedItem.count;
                    }
                    else
                    {
                        Item temp = target;
                        inv.setSlot(slot, draggedItem);
                        inv.setSlot(draggedFromSlot, temp);
                    }
                }
                else
                {
                    inv.setSlot(draggedFromSlot, draggedItem);
                }

                isDragging = false;
                draggedItem = Item{};
                draggedFromSlot = -1;
                draggedFromHotbar = false;
            }
        }
    }   // ← конец while (pollEvent)
}       // ← конец handleEvents

// ============================================
// ОБНОВЛЕНИЕ
// ============================================
void Game::update(float dt)
{
    // День
    dayTimer += dt;
    if (dayTimer >= DAY_LENGTH)
    {
        dayTimer = 0.f;
        currentDay++;
        world.updateGrowth();
    }

    // Игрок
    player.update(dt, 800.f, 600.f);
}

// ============================================
// ОТРИСОВКА
// ============================================
void Game::render()
{
    window.clear(sf::Color::Black);

    // Мир
    world.draw(window);

    // Маркер тайла перед игроком
    {
        sf::Vector2f ppos = player.shape.getPosition();
        float cx = ppos.x + Player::SIZE / 2.f;
        float cy = ppos.y + Player::SIZE / 2.f;
        int tx = static_cast<int>(cx / World::TILE_SIZE) + player.dirX;
        int ty = static_cast<int>(cy / World::TILE_SIZE) + player.dirY;

        if (tx >= 0 && tx < World::WIDTH && ty >= 0 && ty < World::HEIGHT)
        {
            targetMarker.setPosition({ tx * World::TILE_SIZE, ty * World::TILE_SIZE });
            window.draw(targetMarker);
        }
    }

    // Игрок
    player.draw(window);

    // UI
    float dayProgress = dayTimer / DAY_LENGTH;
    ui.drawDayBar(window, dayProgress);
    ui.drawTexts(window, currentDay, inv.countItem("seeds"), inv.countItem("leaves"));
    if (!isInventoryOpen)
    {
        ui.drawHotbar(window, inv, activeSlot);
    }
    ui.drawInventory(window, inv, activeSlot, isInventoryOpen, isDragging);

    // Иконка перетаскивания (под курсором)
    if (isDragging)
    {
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        ui.drawItemIcon(window,
            static_cast<float>(mousePixel.x) - UI::SLOT_SIZE / 2.f,
            static_cast<float>(mousePixel.y) - UI::SLOT_SIZE / 2.f,
            draggedItem);
    }

    window.display();
}


// ============================================
// ПОИСК СЛОТА ПОД МЫШЬЮ
// ============================================
int Game::getSlotUnderMouse(sf::Vector2i mousePos)
{
    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    // Инвентарь закрыт — только нижний хотбар
    if (!isInventoryOpen)
    {
        float hotbarY = 600.f - UI::SLOT_SIZE - UI::HOTBAR_MARGIN;
        for (int i = 0; i < UI::HOTBAR_SLOTS; i++)
        {
            float slotX = UI::HOTBAR_MARGIN + UI::SLOT_PADDING + i * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            if (isInside(mx, my, slotX, hotbarY, UI::SLOT_SIZE))
                return i;
        }
        return -1;
    }

    // Инвентарь открыт — рюкзак + хотбар внутри
    const float GRID_X = 150.f + UI::SLOT_PADDING;
    const float GRID_Y = 180.f + UI::SLOT_PADDING;

    // Рюкзак (слоты 9-35)
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float x = GRID_X + col * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            float y = GRID_Y + row * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            if (isInside(mx, my, x, y, UI::SLOT_SIZE))
                return 9 + row * 9 + col;
        }
    }

    // Хотбар в инвентаре (слоты 0-8)
    float hotbarY = GRID_Y + 3 * (UI::SLOT_SIZE + UI::SLOT_PADDING) + 5.f;
    for (int col = 0; col < 9; col++)
    {
        float x = GRID_X + col * (UI::SLOT_SIZE + UI::SLOT_PADDING);
        if (isInside(mx, my, x, hotbarY, UI::SLOT_SIZE))
            return col;
    }

    // Корзина — специальный индекс -2
    float trashX = 650.f - UI::SLOT_SIZE;
    float trashY = hotbarY + UI::SLOT_SIZE + 5.f;
    if (isInside(mx, my, trashX, trashY, UI::SLOT_SIZE))
        return -2;

    return -1;
}