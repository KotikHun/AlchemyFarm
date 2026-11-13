#pragma once
#include "Player.hpp"
#include "Inventory.hpp"
#include "World.hpp"
#include "House.hpp"
#include "Chest.hpp"
#include "Furniture.hpp"
#include "AlchemySystem.hpp"
#include "SoundManager.hpp"
#include "ItemDatabase.hpp"
#include "FloatingText.hpp"
#include "PlacementSystem.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class PlayerInteractionController
{
public:
    static bool handleUseAction(Player& player,
                                Inventory& inv,
                                int activeSlot,
                                World& world,
                                House& house,
                                std::vector<Chest>& chests,
                                std::vector<Furniture>& furniture,
                                Chest* nearbyChest,
                                int nearbyFurnitureIdx,
                                AlchemySystem& alchemy,
                                PlacementSystem& placement,
                                SoundManager& sound,
                                const ItemDatabase& itemDB,
                                std::vector<FloatingText>& floatingTexts,
                                std::vector<DroppedItem>& droppedItems,
                                bool& isTransitioningSleep,
                                bool isTransitioningWake,
                                float& fadeTarget,
                                struct SleepReport& sleepReport,
                                bool isRaining = false);

    static bool handleDropItem(Player& player,
                               Inventory& inv,
                               int activeSlot,
                               std::vector<DroppedItem>& droppedItems);

    static bool handleConsumeOrUseItem(Player& player,
                                       Inventory& inv,
                                       int activeSlot,
                                       World& world,
                                       House& house,
                                       std::vector<Chest>& chests,
                                       std::vector<Furniture>& furniture,
                                       PlacementSystem& placement,
                                       const ItemDatabase& itemDB,
                                       std::vector<FloatingText>& floatingTexts);
};
