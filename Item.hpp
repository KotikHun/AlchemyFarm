#pragma once

#include <string>

// ============================================
// ПРЕДМЕТ
// ============================================
// Данные о типе предмета (загружаются из items.json).
struct ItemData {
    std::string id;           // "hoe", "seeds"
    std::string name;         // "Мотыга", "Семена"
    int maxStack = 1;         // 1 или 99
    bool isTool = false;      // true = инструмент
    std::string category;     // "Инструмент", "Зелье", "Алхимия", "Мебель"
    std::string description;  // Подробное описание предмета
    std::string effects;      // Описание эффектов и применения
    int sellPrice = 0;        // Стоимость продажи в ящик сбыта (золото)
};

// ============================================
// КОНКРЕТНЫЙ ПРЕДМЕТ В СЛОТЕ
// ============================================
struct Item {
    std::string id;        // "" = пусто
    int count = 0;         // сколько (для стека)

    bool isEmpty() const { return id.empty() || count <= 0; }
};
