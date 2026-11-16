#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include "Furniture.hpp"
#include "Chest.hpp"

// Forward declarations
struct World;
class House;
struct Player;
struct Inventory;
struct ItemDatabase;
struct FloatingText;
struct DroppedItem;

// ============================================
// СИСТЕМА РАЗМЕЩЕНИЯ И ДЕМОНТАЖА МЕБЕЛИ
// ============================================

class PlacementSystem {
public:
    sf::Texture chestTexture;
    std::unique_ptr<sf::Sprite> chestSprite;

    sf::Texture bedTexture;
    std::unique_ptr<sf::Sprite> bedSprite;

    sf::Texture tableTexture;
    std::unique_ptr<sf::Sprite> tableSprite;

    sf::Texture cauldronAnimTexture;
    std::unique_ptr<sf::Sprite> cauldronAnimSprite;

    sf::Texture carpetTexture;
    std::unique_ptr<sf::Sprite> carpetSprite;

    sf::Texture windowTexture;
    std::unique_ptr<sf::Sprite> windowSprite;

    PlacementSystem();

    void loadTextures();

    void drawFurniturePiece(sf::RenderWindow& window, const Furniture& f);

    bool isPlacementValid(const std::string& itemId,
                          int tx,
                          int ty,
                          const House& house,
                          const World& world,
                          const Player& player,
                          const std::vector<Chest>& chests,
                          const std::vector<Furniture>& furniture,
                          std::string* outReason = nullptr) const;

    bool tryPlaceFurnitureAt(const std::string& itemId,
                             int tx,
                             int ty,
                             House& house,
                             const World& world,
                             Player& player,
                             Inventory& inv,
                             std::vector<Chest>& chests,
                             std::vector<Furniture>& furniture,
                             std::vector<FloatingText>& floatingTexts,
                             const ItemDatabase& itemDB);

    bool tryPlaceFurniture(const std::string& itemId,
                           House& house,
                           const World& world,
                           Player& player,
                           Inventory& inv,
                           std::vector<Chest>& chests,
                           std::vector<Furniture>& furniture,
                           std::vector<FloatingText>& floatingTexts,
                           const ItemDatabase& itemDB);

    // Трекинг демонтажа (3 быстрых удара инструментом)
    int hitTargetType = 0; // 0 = none, 1 = chest, 2 = furniture
    size_t hitTargetIndex = 0;
    int hitCount = 0;
    sf::Clock hitTimer;

    bool tryPickupFurniture(const House& house,
                            Player& player,
                            Inventory& inv,
                            std::vector<Chest>& chests,
                            std::vector<Furniture>& furniture,
                            std::vector<FloatingText>& floatingTexts,
                            const ItemDatabase& itemDB,
                            Chest*& nearbyChest,
                            int& nearbyFurnitureIdx,
                            std::vector<DroppedItem>* droppedItems = nullptr);

    void drawPlacementGrid(sf::RenderWindow& window,
                           const Inventory& inv,
                           int activeSlot,
                           const Player& player,
                           const House& house,
                           const World& world,
                           const std::vector<Chest>& chests,
                           const std::vector<Furniture>& furniture,
                           sf::Vector2i mousePix);
};
