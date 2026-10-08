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
            if (keyPressed->code == sf::Keyboard::Key::Num1)
                currentTool = Tool::Hoe;
            else if (keyPressed->code == sf::Keyboard::Key::Num2)
                currentTool = Tool::Seed;
            else if (keyPressed->code == sf::Keyboard::Key::Num3)
                currentTool = Tool::WateringCan;
            else if (keyPressed->code == sf::Keyboard::Key::Num4)
                currentTool = Tool::Hand;

            if (keyPressed->code == sf::Keyboard::Key::E)
            {
                world.interact(player, currentTool, inv);
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
    ui.drawTexts(window, currentDay, inv.seeds, inv.leaves);
    ui.drawHotbar(window, currentTool);

    window.display();
}