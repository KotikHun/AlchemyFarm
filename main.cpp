#include <SFML/Graphics.hpp>
#include <optional>
#include <array>
#include <string>
#include "Tile.hpp"
#include "Inventory.hpp"
#include "Player.hpp"
#include "Tool.hpp"
#include "World.hpp"

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

    const float SLOT_SIZE = 50.f;
    const float SLOT_PADDING = 5.f;
    const float HOTBAR_Y = 600.f - SLOT_SIZE - 10.f;
    const int HOTBAR_SLOTS = 4;  // было 3, стало 4

    // Фон хотбара — тёмная полоса
    sf::RectangleShape hotbarBg({ HOTBAR_SLOTS * SLOT_SIZE + (HOTBAR_SLOTS + 1) * SLOT_PADDING, SLOT_SIZE + 2 * SLOT_PADDING });
    hotbarBg.setFillColor(sf::Color(40, 40, 40));
    hotbarBg.setPosition({ 10.f, HOTBAR_Y - SLOT_PADDING });

    // Слоты — один прямоугольник, переиспользуем
    sf::RectangleShape slotShape({ SLOT_SIZE, SLOT_SIZE });

    Inventory inv;

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
                    // === ВЗАИМОДЕЙСТВИЕ ===

                    // 1. Центр игрока
                    sf::Vector2f pos = player.shape.getPosition();
                    float centerX = pos.x + Player::SIZE / 2.f;
                    float centerY = pos.y + Player::SIZE / 2.f;

                    // 2. Тайл под игроком
                    int tileX = static_cast<int>(centerX / World::TILE_SIZE);
                    int tileY = static_cast<int>(centerY / World::TILE_SIZE);

                    // 3. Тайл перед игроком
                    int targetX = tileX + player.dirX;
                    int targetY = tileY + player.dirY;

                    // 4. Проверка: не выходит ли за границы карты?
                    if (targetX >= 0 && targetX < World::WIDTH &&
                        targetY >= 0 && targetY < World::HEIGHT)
                    {
                        // 5. Ссылка на тайл, с которым работаем
                        Tile& t = world.map[targetY][targetX];

                           // 6. Действие зависит от выбранного инструмента
                            switch (currentTool)
                            {
                            case Tool::Hoe:
                                // Мотыга: 
                                // - трава → грядка
                                // - грядка с растением → убрать
                                if (t.type == 0)
                                {
                                    t.type = 1;
                                }
                                else if (t.type == 1 && t.plantStage > 0)
                                {
                                    // Спелое — упал урожай
                                    if (t.plantStage == 3)
                                    {
                                        inv.leaves++;
                                    }
                                    // Молодое — просто пропадает
                                    t.plantStage = 0;
                                    t.watered = false;
                                }
                                break;

                            case Tool::Seed:
                                // Семечко: пустая грядка + есть семена → посадить
                                if (t.type == 1 && t.plantStage == 0 && inv.seeds > 0)
                                {
                                    t.plantStage = 1;
                                    inv.seeds--;
                                }
                                break;

                            case Tool::WateringCan:
                                // Лейка: любая вскопанная грядка, не политая → полить
                                if (t.type == 1 && !t.watered)
                                {
                                    t.watered = true;
                                }
                                break;

                            case Tool::Hand:
                                // Рука: собрать спелое растение
                                if (t.type == 1 && t.plantStage == 3)
                                {
                                    inv.leaves++;
                                    t.plantStage = 0;
                                    t.watered = false;
                                }
                                break;
                            }
                    }
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
        window.draw(hotbarBg);

        for (int i = 0; i < HOTBAR_SLOTS; i++)
        {
            float slotX = 10.f + SLOT_PADDING + i * (SLOT_SIZE + SLOT_PADDING);
            float slotY = HOTBAR_Y;
            slotShape.setPosition({ slotX, slotY });

            // Выбранный слот — светлый, остальные — тёмные
            bool isSelected = false;
            if (i == 0 && currentTool == Tool::Hoe) isSelected = true;
            if (i == 1 && currentTool == Tool::Seed) isSelected = true;
            if (i == 2 && currentTool == Tool::WateringCan) isSelected = true;
            if (i == 3 && currentTool == Tool::Hand) isSelected = true;

            if (isSelected)
                slotShape.setFillColor(sf::Color(200, 200, 100));  // жёлтоватый
            else
                slotShape.setFillColor(sf::Color(80, 80, 80));     // серый

            window.draw(slotShape);
        }

        window.display();
    }

    return 0;
}