#pragma once
#include <SFML/Graphics.hpp>
#include "Inventory.hpp"

struct Chest {
    Inventory inventory;
    int slotCount = 18;
    int columns = 9;
    sf::Vector2f position;
    bool isOpen = false;
    std::string type = "wooden";
    bool isInside = false;
};
