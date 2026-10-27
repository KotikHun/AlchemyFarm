#include "House.hpp"
#include <iostream>
#include <cmath>

House::House()
{
    // ============================================
    // 1. ЛОКАЦИЯ 1: ВНЕШНИЙ МИР (ДОМ СНАРУЖИ)
    // ============================================
    // Загрузка спрайта дома: Pack 5 1 вариант
    if (exteriorTexture.loadFromFile("assets/sprites/house_pack5_var1_clean.png"))
    {
        exteriorSprite = std::make_unique<sf::Sprite>(exteriorTexture);
        // Точка привязки (Pivot Point): строго по центру снизу (Bottom Center)
        // Исходный спрайт: 144x144 px -> центр по X: 72.f, низ по Y: 144.f
        exteriorSprite->setOrigin({ 72.f, 144.f });
        exteriorSprite->setScale({ 2.0f, 2.0f });
        exteriorSprite->setPosition(exteriorPos);
    }
    else
    {
        std::cerr << "House: failed to load house_pack5_var1_clean.png" << std::endl;
    }

    // ============================================
    // 2. ЛОКАЦИЯ 2: ИНТЕРЬЕР ДОМА (ОТДЕЛЬНАЯ СЦЕНА)
    // ============================================
    // А) Спрайт пола (увеличенный масштаб деревянных плашек для уюта)
    if (floorTexture.loadFromFile("assets/sprites/interior_floor_stardew.png") ||
        floorTexture.loadFromFile("assets/sprites/interior_floor_dark.png"))
    {
        floorTexture.setRepeated(true);
        floorSprite = std::make_unique<sf::Sprite>(floorTexture);
        floorSprite->setScale({ 1.5f, 1.5f });
        floorSprite->setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(ROOM_W / 1.5f), static_cast<int>(ROOM_H / 1.5f) }));
        floorSprite->setPosition({ ROOM_X, ROOM_Y });
    }
    else
    {
        std::cerr << "House: failed to load floor texture" << std::endl;
    }

    // Б) Кровать: деревянная уютная кровать Stardew Valley (32x55 px при масштабе 1.5x)
    if (bedTexture.loadFromFile("assets/sprites/bed_wooden.png") ||
        bedTexture.loadFromFile("assets/sprites/black_bed_final.png"))
    {
        bedSprite = std::make_unique<sf::Sprite>(bedTexture);
        // Точка привязки (Pivot Point): строго по центру снизу (Bottom Center)
        bedSprite->setOrigin({ 16.f, 55.f });
        bedSprite->setScale({ 1.5f, 1.5f });
        bedSprite->setPosition(bedPos);
    }
    else
    {
        std::cerr << "House: failed to load bed texture" << std::endl;
    }
}

void House::update(float dt)
{
    // Отдельная сцена спальни чистая — без лишних старых анимаций
}

void House::drawExterior(sf::RenderWindow& window)
{
    if (exteriorSprite)
    {
        exteriorSprite->setPosition(exteriorPos);
        window.draw(*exteriorSprite);
    }
}

void House::drawInterior(sf::RenderWindow& window)
{
    // --- 1. СЛОЙ ПОЛА (Sorting Layer: Floor) ---
    // Пол рисуется первым, гарантированно под ногами игрока и под мебелью
    if (floorSprite)
    {
        window.draw(*floorSprite);
    }
    else
    {
        sf::RectangleShape fallbackFloor({ ROOM_W, ROOM_H });
        fallbackFloor.setPosition({ ROOM_X, ROOM_Y });
        fallbackFloor.setFillColor(sf::Color(45, 30, 20));
        window.draw(fallbackFloor);
    }

    // Темная окантовка вокруг пола (границы стен комнаты)
    sf::RectangleShape border({ ROOM_W, ROOM_H });
    border.setPosition({ ROOM_X, ROOM_Y });
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(35, 25, 18));
    border.setOutlineThickness(6.f);
    window.draw(border);

    // Внутренний коврик/порог у двери выхода
    sf::RectangleShape doorMat({ 54.f, 14.f });
    doorMat.setPosition({ exitDoorPos.x - 27.f, exitDoorPos.y - 12.f });
    doorMat.setFillColor(sf::Color(65, 45, 30));
    doorMat.setOutlineColor(sf::Color(90, 65, 45));
    doorMat.setOutlineThickness(1.5f);
    window.draw(doorMat);

    // Слой мебели и предметов теперь динамически управляется Game (кровать, сундук, стол, ковер, окно)
}

std::vector<sf::FloatRect> House::getExteriorObstacles() const
{
    // Физика снаружи: персонаж может свободно подниматься по лестнице к коврику перед дверью,
    // но останавливается перед дверью и не может зайти в стены дома, окно или закрытую дверь.
    return {
        // 1. Вся стена дома по ширине: блокирует проход дальше порога (y - 48.f)
        sf::FloatRect({ exteriorPos.x - 144.f, exteriorPos.y - 144.f }, { 288.f, 96.f }),

        // 2. Перила и левая часть веранды на уровне ступенек
        sf::FloatRect({ exteriorPos.x - 144.f, exteriorPos.y - 48.f }, { 150.f, 48.f }),

        // 3. Перила и правая часть веранды на уровне ступенек
        sf::FloatRect({ exteriorPos.x + 58.f, exteriorPos.y - 48.f }, { 86.f, 48.f })
    };
}

std::vector<sf::FloatRect> House::getInteriorObstacles() const
{
    return {
        // Внешние стены комнаты (сплошная стена снизу блокирует выход за пределы пола в пустоту)
        sf::FloatRect({ ROOM_X - 20.f, ROOM_Y - 20.f }, { ROOM_W + 40.f, 25.f }),    // Верх
        sf::FloatRect({ ROOM_X - 20.f, ROOM_Y }, { 25.f, ROOM_H }),                   // Лево
        sf::FloatRect({ ROOM_X + ROOM_W - 5.f, ROOM_Y }, { 25.f, ROOM_H }),           // Право
        sf::FloatRect({ ROOM_X - 20.f, ROOM_Y + ROOM_H - 6.f }, { ROOM_W + 40.f, 30.f }) // Сплошной низ (игрок не выходит за пределы комнаты)
    };
}

bool House::isNearExteriorDoor(const sf::Vector2f& playerPos) const
{
    // Дверь и зеленый коврик на крыльце: x = exteriorPos.x + 32.f, y = exteriorPos.y - 44.f
    sf::Vector2f doorCenter = { exteriorPos.x + 32.f, exteriorPos.y - 44.f };
    float dx = playerPos.x - doorCenter.x;
    float dy = playerPos.y - doorCenter.y;
    return (dx * dx + dy * dy) < (45.f * 45.f);
}

bool House::isNearExitDoor(const sf::Vector2f& playerPos) const
{
    // Триггер выхода внутри дома (у порога)
    float dx = playerPos.x - exitDoorPos.x;
    float dy = playerPos.y - exitDoorPos.y;
    return (dx * dx + dy * dy) < (45.f * 45.f);
}

bool House::isNearBed(const sf::Vector2f& playerPos) const
{
    // Зона взаимодействия с кроватью в верхнем углу (сон)
    sf::Vector2f bedCenter = { bedPos.x, bedPos.y - 35.f };
    float dx = playerPos.x - bedCenter.x;
    float dy = playerPos.y - bedCenter.y;
    return (dx * dx + dy * dy) < (55.f * 55.f);
}

void House::enter(Player& player)
{
    // Загрузка сцены "Интерьер дома" + телепорт игрока к внутренней двери
    isInside = true;
    player.shape.setPosition({ exitDoorPos.x - 16.f, exitDoorPos.y - 50.f });
    player.dirX = 0;
    player.dirY = -1; // Взгляд вверх в комнату
}

void House::exit(Player& player)
{
    // Загрузка сцены "Внешний мир / Улица" + телепорт игрока на коврик перед дверью
    isInside = false;
    player.shape.setPosition({ exteriorPos.x + 32.f - 16.f, exteriorPos.y - 40.f });
    player.dirX = 0;
    player.dirY = 1; // Взгляд вниз по лестнице
}
