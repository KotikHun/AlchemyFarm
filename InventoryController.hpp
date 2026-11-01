#pragma once
#include "Inventory.hpp"
#include "Chest.hpp"
#include "Player.hpp"
#include "SoundManager.hpp"
#include "ItemDatabase.hpp"
#include "FloatingText.hpp"
#include "UI.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class InventoryController
{
public:
    static int getSlotUnderMouse(sf::Vector2i mousePos, bool chestOpen);
    static int getChestSlotUnderMouse(sf::Vector2i mousePos, const Chest* nearbyChest);

    static bool handleTabClicks(float mx, float my,
                                InventoryTab& currentTab,
                                int& journalCategory,
                                SoundManager& sound);

    static bool handleCraftingClick(float mx, float my,
                                    Inventory& inv,
                                    Player& player,
                                    SoundManager& sound,
                                    std::vector<FloatingText>& floatingTexts);

    static bool handleDragStart(float mx, float my,
                                sf::Vector2i mousePixel,
                                bool isShift,
                                bool isInventoryOpen,
                                Chest* nearbyChest,
                                Inventory& inv,
                                const ItemDatabase& itemDB,
                                std::vector<FloatingText>& floatingTexts,
                                sf::Vector2f playerPos,
                                bool& isDragging,
                                Item& draggedItem,
                                int& draggedFromSlot,
                                bool& draggedFromChest,
                                bool& draggedFromHotbar);

    static bool handleDragRelease(float mx, float my,
                                  sf::Vector2i mousePixel,
                                  bool& isDragging,
                                  Item& draggedItem,
                                  int& draggedFromSlot,
                                  bool& draggedFromChest,
                                  Chest* nearbyChest,
                                  Inventory& inv,
                                  const ItemDatabase& itemDB,
                                  SoundManager& sound,
                                  std::vector<FloatingText>& floatingTexts,
                                  sf::Vector2f playerPos);
};
