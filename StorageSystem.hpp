#pragma once
#include "Inventory.hpp"
#include "Chest.hpp"
#include "ItemDatabase.hpp"
#include "FloatingText.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class StorageSystem
{
public:
    static void transferToChest(Inventory& srcInv,
                                int srcSlot,
                                Chest& chest,
                                const ItemDatabase& itemDB,
                                std::vector<FloatingText>& floatingTexts,
                                sf::Vector2f playerPos);

    static void transferToPlayer(Chest& chest,
                                 int chestSlot,
                                 Inventory& playerInv,
                                 const ItemDatabase& itemDB);
};
