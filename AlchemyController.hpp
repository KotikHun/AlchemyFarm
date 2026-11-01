#pragma once
#include "AlchemySystem.hpp"
#include "Inventory.hpp"
#include "SoundManager.hpp"
#include "ItemDatabase.hpp"
#include "FloatingText.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class AlchemyController
{
public:
    static bool handleCauldronClick(float mx, float my,
                                    AlchemySystem& alchemy,
                                    Inventory& inv,
                                    SoundManager& sound,
                                    const ItemDatabase& itemDB,
                                    std::vector<FloatingText>& floatingTexts);

    static bool handleTableClick(float mx, float my,
                                 AlchemySystem& alchemy,
                                 Inventory& inv,
                                 SoundManager& sound,
                                 const ItemDatabase& itemDB,
                                 std::vector<FloatingText>& floatingTexts);

    static bool handleRightClick(float mx, float my,
                                 const AlchemySystem& alchemy,
                                 const Inventory& inv,
                                 std::string& inspectedItemId);
};
