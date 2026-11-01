#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Item.hpp"
#include "DroppedItem.hpp"

// ============================================
// ДЕРЕВО (OAK TREE)
// ============================================
// Большой дуб (128x160 пикселей).
// При проходе за кроной становится полупрозрачным.
// Рубится топором за 4 удара.

struct Tree {
    sf::Vector2f position; // верхний левый угол спрайта
    int hp = 4;
    static constexpr float WIDTH = 128.f;
    static constexpr float HEIGHT = 160.f;

    // Создание дерева с привязкой основания ствола ровно к 1 тайлу (32x32)
    static Tree createAtTile(int trunkTx, int trunkTy, int hp = 4) {
        Tree t;
        t.hp = hp;
        t.position = sf::Vector2f(trunkTx * 32.f - 48.f, trunkTy * 32.f - 110.f);
        return t;
    }

    // Коллизия ствола — ровно 1 тайл (32x32), откуда растёт ствол
    sf::FloatRect getTrunkBounds() const {
        return sf::FloatRect(
            { position.x + 48.f, position.y + 110.f },
            { 32.f, 32.f }
        );
    }

    // Находится ли игрок за пышной кроной
    bool isBehindCrown(float px, float py) const {
        return (px >= position.x + 10.f && px <= position.x + WIDTH - 10.f &&
                py >= position.y + 5.f && py <= position.y + 120.f);
    }
};
