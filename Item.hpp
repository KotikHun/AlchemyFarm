#pragma once

#include <string>

// ============================================
// ПРЕДМЕТ
// ============================================
// Данные о типе предмета (загружаются из items.json).
struct ItemData {
    std::string id;        // "hoe", "seeds"
    std::string name;      // "Мотыга", "Семена"
    int maxStack = 1;      // 1 или 99
    bool isTool = false;   // true = инструмент
};

// ============================================
// КОНКРЕТНЫЙ ПРЕДМЕТ В СЛОТЕ
// ============================================
struct Item {
    std::string id;        // "" = пусто
    int count = 0;         // сколько (для стека)

    bool isEmpty() const { return id.empty() || count <= 0; }
};