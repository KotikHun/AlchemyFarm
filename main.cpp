#include <SFML/Graphics.hpp>
#include <optional>
#include <array>

// Тайл карты
struct Tile {
    int type = 0;          // 0 = трава, 1 = грядка
    int plantStage = 0;    // 0 = пусто, 1 = семечко, 2 = росток, 3 = взрослое
    bool watered = false;  // полито ли
    int daysGrown = 0;     // сколько дней растёт
};

// Инструменты
enum class Tool {
    Hoe,          // 0 — мотыга
    Seed,         // 1 — семечко
    WateringCan   // 2 — лейка
};

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Alchemy Farm");

    // ============================================
 // КАРТА
 // ============================================
    const int MAP_WIDTH = 30;
    const int MAP_HEIGHT = 25;
    const float TILE_SIZE = 32.f;

    // 0 = трава, 1 = грядка
    std::array<std::array<Tile, MAP_WIDTH>, MAP_HEIGHT> map;

    // Заполняем всю карту травой
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            map[y][x].type = 0;
        }
    }

    sf::RectangleShape tileShape({ TILE_SIZE, TILE_SIZE });

    const sf::Color GRASS_COLOR(60, 130, 60);
    const sf::Color SOIL_COLOR(139, 90, 43);

    // ============================================
    // ИГРОК
    // ============================================
    const float PLAYER_SIZE = 24.f;
    const float PLAYER_SPEED = 150.f;

    sf::RectangleShape player({ PLAYER_SIZE, PLAYER_SIZE });
    player.setFillColor(sf::Color(70, 130, 220));
    player.setPosition({ 100.f, 100.f });

    // Направление взгляда игрока (0, -1 = вверх)
    int dirX = 0;
    int dirY = -1;

    // Маркер — куда сейчас нацелен игрок (для отладки/визуализации)
    sf::RectangleShape targetMarker({ TILE_SIZE, TILE_SIZE });
    targetMarker.setFillColor(sf::Color(255, 255, 0, 100));  // полупрозрачный жёлтый

    // ============================================
    // ХОТБАР
    // ============================================
    Tool currentTool = Tool::Hoe;  // по умолчанию — мотыга

    const float SLOT_SIZE = 50.f;
    const float SLOT_PADDING = 5.f;
    const float HOTBAR_Y = 600.f - SLOT_SIZE - 10.f;  // 10 пикселей от низа

    // Фон хотбара — тёмная полоса
    sf::RectangleShape hotbarBg({ 3 * SLOT_SIZE + 4 * SLOT_PADDING, SLOT_SIZE + 2 * SLOT_PADDING });
    hotbarBg.setFillColor(sf::Color(40, 40, 40));
    hotbarBg.setPosition({ 10.f, HOTBAR_Y - SLOT_PADDING });

    // Слоты — один прямоугольник, переиспользуем
    sf::RectangleShape slotShape({ SLOT_SIZE, SLOT_SIZE });

    // ============================================
// ИНВЕНТАРЬ (пока просто счётчик)
// ============================================
    int seeds = 5;   // стартовые семена
    int leaves = 0;  // собранные листья

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

            // Пройти по всем тайлам и вырастить политые растения
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                for (int x = 0; x < MAP_WIDTH; x++)
                {
                    Tile& t = map[y][x];

                    // Если полито и есть растение, но ещё не взрослое — растём
                    if (t.watered && t.plantStage > 0 && t.plantStage < 3)
                    {
                        t.plantStage++;
                    }

                    // Сброс полива — утром надо полить снова
                    t.watered = false;
                }
            }
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

                if (keyPressed->code == sf::Keyboard::Key::E)
                {
                    // === ВЗАИМОДЕЙСТВИЕ ===

                    // 1. Центр игрока
                    sf::Vector2f pos = player.getPosition();
                    float centerX = pos.x + PLAYER_SIZE / 2.f;
                    float centerY = pos.y + PLAYER_SIZE / 2.f;

                    // 2. Тайл под игроком
                    int tileX = static_cast<int>(centerX / TILE_SIZE);
                    int tileY = static_cast<int>(centerY / TILE_SIZE);

                    // 3. Тайл перед игроком
                    int targetX = tileX + dirX;
                    int targetY = tileY + dirY;

                    // 4. Проверка: не выходит ли за границы карты?
                    if (targetX >= 0 && targetX < MAP_WIDTH &&
                        targetY >= 0 && targetY < MAP_HEIGHT)
                    {
                        // 5. Ссылка на тайл, с которым работаем
                        Tile& t = map[targetY][targetX];

                        // 6. Действие зависит от выбранного инструмента
                        switch (currentTool)
                        {
                        case Tool::Hoe:
                            // Мотыга: трава (0) → грядка (1)
                            if (t.type == 0)
                            {
                                t.type = 1;
                            }
                            break;

                        case Tool::Seed:
                            // Семечко: пустая грядка + есть семена → посадить
                            if (t.type == 1 && t.plantStage == 0 && seeds > 0)
                            {
                                t.plantStage = 1;
                                seeds--;
                            }
                            break;

                        case Tool::WateringCan:
                            // Лейка: любая вскопанная грядка, не политая → полить
                            if (t.type == 1 && !t.watered)
                                t.watered = true;
                            break;
                        }
                    }
                }
            }
        }

        // ============================================
        // ОБНОВЛЕНИЕ
        // ============================================
        sf::Vector2f pos = player.getPosition();
        float dx = 0.f;
        float dy = 0.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { dy -= PLAYER_SPEED * dt; dirX = 0; dirY = -1; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { dy += PLAYER_SPEED * dt; dirX = 0; dirY = 1; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { dx -= PLAYER_SPEED * dt; dirX = -1; dirY = 0; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { dx += PLAYER_SPEED * dt; dirX = 1; dirY = 0; }

        pos.x += dx;
        pos.y += dy;

        float maxX = 800.f - PLAYER_SIZE;
        float maxY = 600.f - PLAYER_SIZE;

        if (pos.x < 0.f) pos.x = 0.f;
        if (pos.y < 0.f) pos.y = 0.f;
        if (pos.x > maxX) pos.x = maxX;
        if (pos.y > maxY) pos.y = maxY;

        player.setPosition(pos);

        // ============================================
        // ОТРИСОВКА
        // ============================================
        window.clear(sf::Color::Black);

        // Карта
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                float px = x * TILE_SIZE;
                float py = y * TILE_SIZE;

                tileShape.setPosition({ px, py });

                // Ссылка на тайл
                Tile& t = map[y][x];

                // Цвет тайла зависит от типа и полива
                if (t.type == 0)
                {
                    tileShape.setFillColor(GRASS_COLOR);
                }
                else if (t.type == 1)
                {
                    if (t.watered)
                        tileShape.setFillColor(sf::Color(100, 65, 30));  // тёмно-коричневый
                    else
                        tileShape.setFillColor(SOIL_COLOR);
                }

                window.draw(tileShape);

                // Растение (если посажено)
                if (t.plantStage > 0)
                {
                    float cx = px + TILE_SIZE / 2.f;
                    float cy = py + TILE_SIZE / 2.f;

                    // Размер и цвет зависят от стадии
                    float radius = 4.f;
                    sf::Color plantColor(60, 130, 60);  // тёмно-зелёный (семечко)

                    if (t.plantStage == 2)
                    {
                        radius = 7.f;
                        plantColor = sf::Color(100, 180, 100);  // средне-зелёный
                    }
                    else if (t.plantStage == 3)
                    {
                        radius = 10.f;
                        plantColor = sf::Color(150, 220, 100);  // ярко-салатовый (спелое)
                    }

                    sf::CircleShape plant(radius);
                    plant.setFillColor(plantColor);
                    plant.setOrigin({ radius, radius });
                    plant.setPosition({ cx, cy });
                    window.draw(plant);
                }
            }
        }

        // Маркер на тайле перед игроком
        {
            sf::Vector2f ppos = player.getPosition();
            float cx = ppos.x + PLAYER_SIZE / 2.f;
            float cy = ppos.y + PLAYER_SIZE / 2.f;
            int tx = static_cast<int>(cx / TILE_SIZE) + dirX;
            int ty = static_cast<int>(cy / TILE_SIZE) + dirY;

            if (tx >= 0 && tx < MAP_WIDTH && ty >= 0 && ty < MAP_HEIGHT)
            {
                targetMarker.setPosition({ tx * TILE_SIZE, ty * TILE_SIZE });
                window.draw(targetMarker);
            }
        }

        window.draw(player);

        // ============================================
// ПОЛОСКА ДНЯ
// ============================================
        window.draw(dayBarBg);

        // Заполнение = сколько прошло от дня
        float dayProgress = dayTimer / DAY_LENGTH;
        dayBar.setSize({ 800.f * dayProgress, 8.f });
        window.draw(dayBar);

        // ============================================
        // ХОТБАР
        // ============================================
        window.draw(hotbarBg);

        for (int i = 0; i < 3; i++)
        {
            float slotX = 10.f + SLOT_PADDING + i * (SLOT_SIZE + SLOT_PADDING);
            float slotY = HOTBAR_Y;
            slotShape.setPosition({ slotX, slotY });

            // Выбранный слот — светлый, остальные — тёмные
            bool isSelected = false;
            if (i == 0 && currentTool == Tool::Hoe) isSelected = true;
            if (i == 1 && currentTool == Tool::Seed) isSelected = true;
            if (i == 2 && currentTool == Tool::WateringCan) isSelected = true;

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