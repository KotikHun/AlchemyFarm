#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Item.hpp"
#include "DroppedItem.hpp"

// ============================================
// ДЕРЕВО (OAK TREE)
// ============================================
// Стадии роста:
// 0: Саженец (sapling, посаженный желудь дал росток, 1-3 день)
// 1: Молодое дерево (medium tree, 4-6 день)
// 2: Взрослый вековой дуб (mature oak, 7+ день, 4 удара топором)

struct Tree {
    sf::Vector2f position; // верхний левый угол спрайта для взрослого дуба
    int hp = 4;
    int stage = 2; // 0, 1, 2
    int daysGrown = 7;
    static constexpr float WIDTH = 128.f;
    static constexpr float HEIGHT = 160.f;

    // Создание дерева с привязкой основания ствола ровно к 1 тайлу (32x32)
    static Tree createAtTile(int trunkTx, int trunkTy, int hp = 4, int stage = 2) {
        Tree t;
        t.hp = hp;
        t.stage = stage;
        t.daysGrown = (stage == 2) ? 7 : 0;
        t.position = sf::Vector2f(trunkTx * 32.f - 48.f, trunkTy * 32.f - 110.f);
        return t;
    }

    // Коллизия ствола — ровно 1 тайл (32x32), откуда растёт ствол
    // Саженцы (0) и молодые деревья (1) полностью проходимы!
    // Только взрослый вековой дуб (2) имеет твердый непроходимый ствол.
    bool isSolid() const {
        return stage == 2;
    }

    sf::FloatRect getTrunkBounds() const {
        return sf::FloatRect(
            { position.x + 48.f, position.y + 110.f },
            { 32.f, 32.f }
        );
    }

    // Находится ли игрок за пышной кроной
    bool isBehindCrown(float px, float py) const {
        if (stage != 2) return false;
        return (px >= position.x + 10.f && px <= position.x + WIDTH - 10.f &&
                py >= position.y + 5.f && py <= position.y + 120.f);
    }
};
