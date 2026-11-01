#pragma once

#include <SFML/Graphics.hpp>
#include <string>

struct Furniture {
    std::string id;        // "bed", "alchemist_table", "cauldron", "carpet"
    sf::Vector2f position;
    bool isInside = false;

    float animTimer = 0.f;
    int animFrame = 0;

    sf::FloatRect getObstacle() const {
        if (id == "carpet") {
            return sf::FloatRect({ 0.f, 0.f }, { 0.f, 0.f });
        }
        if (id == "bed") {
            // Bed origin {16, 55}, scale 1.5x -> 48x82. Bottom Center at position
            return sf::FloatRect({ position.x - 24.f, position.y - 75.f }, { 48.f, 75.f });
        }
        if (id == "alchemist_table") {
            // Frame is 48x64. Table base and obstacle at bottom
            return sf::FloatRect({ position.x, position.y + 24.f }, { 48.f, 40.f });
        }
        if (id == "cauldron") {
            // Ровно 1 тайл 32x32
            return sf::FloatRect(position, { 32.f, 32.f });
        }
        return sf::FloatRect(position, { 32.f, 32.f });
    }

    sf::FloatRect getBounds() const {
        if (id == "bed") {
            return sf::FloatRect({ position.x - 24.f, position.y - 82.5f }, { 48.f, 82.5f });
        }
        if (id == "carpet") {
            return sf::FloatRect(position, { 64.f, 96.f });
        }
        if (id == "alchemist_table") {
            return sf::FloatRect(position, { 48.f, 64.f });
        }
        if (id == "cauldron") {
            // Ровно 1 тайл 32x32
            return sf::FloatRect(position, { 32.f, 32.f });
        }
        return sf::FloatRect(position, { 32.f, 32.f });
    }

    float getSortY() const {
        if (id == "bed") return position.y;
        if (id == "alchemist_table") return position.y + 60.f;
        if (id == "cauldron") return position.y + 32.f;
        return position.y + 32.f;
    }
};
