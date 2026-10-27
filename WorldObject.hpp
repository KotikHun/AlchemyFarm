#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// ============================================
// ОБЪЕКТ МИРА (ПРЕПЯТСТВИЕ / РЕСУРС)
// ============================================
// Сорняки, камни, пни для расчистки фермы

enum class ObjectType {
    Weed,   // Сорняк (рубится мотыгой/рукой, 1 HP, дает волокно/семена, проходимый)
    Rock,   // Камень (бьется киркой, 2 HP, дает камень, твердый)
    Stump   // Пень / полено (рубится топором, 2 HP, дает дерево, твердый)
};

struct WorldObject {
    int tileX = 0;
    int tileY = 0;
    ObjectType type = ObjectType::Weed;
    int hp = 1;
    int maxHp = 1;
    int variant = 0;

    bool isSolid() const {
        return type != ObjectType::Weed;
    }

    sf::FloatRect getBounds(float tileSize = 32.f) const {
        // Компактный хитбокс в центре камня (позволяет спокойно проходить рядом)
        return sf::FloatRect(
            { static_cast<float>(tileX) * tileSize + 4.f, static_cast<float>(tileY) * tileSize + 6.f },
            { tileSize - 8.f, tileSize - 10.f }
        );
    }
};
