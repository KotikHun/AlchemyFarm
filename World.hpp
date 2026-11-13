#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <memory>
#include "Tile.hpp"
#include "Player.hpp"
#include "Inventory.hpp"
#include "WorldObject.hpp"
#include "Tree.hpp"
#include "DroppedItem.hpp"

// ============================================
// ИГРОВОЙ МИР (КАРТА)
// ============================================
// Хранит тайлы, водоем, объекты расчистки, деревья и посевы.

struct World {
    // Спрайт травы
    sf::Texture grassTexture;
    std::unique_ptr<sf::Sprite> grassSprite;

    // Спрайт грядки
    sf::Texture farmlandTexture;
    std::unique_ptr<sf::Sprite> farmlandSprite;

    // Спрайт воды
    sf::Texture waterTexture;
    std::unique_ptr<sf::Sprite> waterSprite;

    // Спрайты декораций (камни, сорняки, пни)
    sf::Texture decorTexture;
    std::unique_ptr<sf::Sprite> decorSprite;

    // Спрайт дерева (Oak Tree 128x160)
    sf::Texture treeTexture;
    std::unique_ptr<sf::Sprite> treeSprite;

    // Спрайт саженца дуба (Sapling 32x32)
    sf::Texture saplingTexture;
    std::unique_ptr<sf::Sprite> saplingSprite;

    // Спрайт посевов (Spring Crops, 4 стадии роста)
    sf::Texture cropsTexture;
    std::unique_ptr<sf::Sprite> cropsSprite;

    // Размер карты
    static constexpr int WIDTH = 30;
    static constexpr int HEIGHT = 25;
    static constexpr float TILE_SIZE = 32.f;

    std::array<std::array<Tile, WIDTH>, HEIGHT> map;
    std::vector<WorldObject> objects;
    std::vector<Tree> trees;

    World();
    void generateNewMap(int seed = 0);

    void draw(sf::RenderWindow& window);
    void drawTrees(sf::RenderWindow& window, const Player& player);
    void updateGrowth();

    WorldObject* getObjectAt(int tx, int ty);
    bool removeObjectAt(int tx, int ty);

    std::vector<sf::FloatRect> getObstacles() const;

    void interact(Player& player, const std::string& itemId, Inventory& inv, std::vector<DroppedItem>& droppedItems);

    sf::IntRect getFarmlandRect(int x, int y) const;
    sf::IntRect getWaterRect(int x, int y) const;
};
