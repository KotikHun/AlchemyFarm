#include "Game.hpp"
#include <iostream>

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

            // Взаимодействие: берём id предмета из активного слота
            if (keyPressed->code == sf::Keyboard::Key::E)
            {
                const Item& activeItem = inv.getSlot(activeSlot);
                world.interact(player, activeItem.id, inv);
            }
        }
    }
}

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

        if (tx >= 0 && tx < World::WIDTH && ty >= 0 && ty < World::HEIGHT);
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
    ui.drawHotbar(window, inv, activeSlot);

    window.display();
}