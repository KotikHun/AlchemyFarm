#include <SFML/Graphics.hpp>
#include <optional>
#include <array>
#include <string>
#include "Tile.hpp"
#include "Inventory.hpp"
#include "Player.hpp"
#include "Tool.hpp"
#include "World.hpp"
#include "UI.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Alchemy Farm");

    // ============================================
    // КАРТА
    // ============================================
    World world;

// ============================================
// ИГРОК
// ============================================
    Player player;

    // Маркер — куда сейчас нацелен игрок (для отладки/визуализации)
    sf::RectangleShape targetMarker({ World::TILE_SIZE, World::TILE_SIZE });
    targetMarker.setFillColor(sf::Color(255, 255, 0, 100));  // полупрозрачный жёлтый

// ============================================
// ШРИФТ И UI
// ============================================
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Roboto-Regular.ttf"))
    {
        return -1;  // если шрифт не загрузился — выход
    }

    // Текст: день
    sf::Text dayText(font);
    dayText.setCharacterSize(20);
    dayText.setFillColor(sf::Color::White);
    dayText.setPosition({ 10.f, 20.f });

    // Текст: семечки
    sf::Text seedsText(font);
    seedsText.setCharacterSize(20);
    seedsText.setFillColor(sf::Color::White);
    seedsText.setPosition({ 10.f, 45.f });

    // Текст: листья
    sf::Text leavesText(font);
    leavesText.setCharacterSize(20);
    leavesText.setFillColor(sf::Color::White);
    leavesText.setPosition({ 10.f, 70.f });


    // ============================================
    // ХОТБАР
    // ============================================
    Tool currentTool = Tool::Hoe;  // по умолчанию — мотыга

   
    Inventory inv;
    UI ui;
    // ============================================
    // ДЕНЬ И ВРЕМЯ
    // ============================================
    const float DAY_LENGTH = 60.f;  // 30 секунд = 1 день (для теста)
    float dayTimer = 0.f;           // сколько секунд прошло с начала дня
    int currentDay = 1;             // номер текущего дня

    // Полоска времени вверху экрана
    sf::RectangleShape dayBarBg({ 800.f, 8.f });
    dayBarBg.setFillColor(sf::Color(40, 40, 40));
    dayBarBg.setPosition({ 0.f, 0.f });

    sf::RectangleShape dayBar({ 800.f, 8.f });
    dayBar.setFillColor(sf::Color(220, 180, 60));  // жёлтый
    dayBar.setPosition({ 0.f, 0.f });

    sf::Clock clock;

    // ============================================
    // ИГРОВОЙ ЦИКЛ
    // ============================================
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        // ============================================
// ДЕНЬ
// ============================================
        dayTimer += dt;

        if (dayTimer >= DAY_LENGTH)
        {
            dayTimer = 0.f;
            currentDay++;

            // Растения растут
            world.updateGrowth();
        }

        // ============================================
        // СОБЫТИЯ
        // ============================================
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Одноразовое нажатие клавиши
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
                    // Взаимодействие с тайлом перед игроком
                    world.interact(player, currentTool, inv);
                }
            }
        }

        // ============================================
        // ОБНОВЛЕНИЕ
        // ============================================
        player.update(dt, 800.f, 600.f);

        // ============================================
        // ОТРИСОВКА
        // ============================================
        window.clear(sf::Color::Black);

        // Карта
        world.draw(window);
        // Маркер на тайле перед игроком
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

        player.draw(window);

        // ============================================
// ПОЛОСКА ДНЯ
// ============================================
        window.draw(dayBarBg);

        // Заполнение = сколько прошло от дня
        float dayProgress = dayTimer / DAY_LENGTH;
        dayBar.setSize({ 800.f * dayProgress, 8.f });
        window.draw(dayBar);

        // ============================================
// ТЕКСТ UI
// ============================================
        dayText.setString("Day: " + std::to_string(currentDay));
        window.draw(dayText);

        seedsText.setString("Seeds: " + std::to_string(inv.seeds));
        window.draw(seedsText);

        leavesText.setString("Leaves: " + std::to_string(inv.leaves));
        window.draw(leavesText);

        // ============================================
// ХОТБАР
// ============================================
        ui.drawHotbar(window, currentTool);

        window.display();
    }

    return 0;
}