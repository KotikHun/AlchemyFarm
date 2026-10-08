#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Item.hpp"

// ============================================
// БАЗА ПРЕДМЕТОВ
// ============================================
// Загружает items.json и предоставляет доступ
// к данным предметов по id.

struct ItemDatabase {
    std::vector<ItemData> items;
    std::unordered_map<std::string, int> idToIndex;  // "hoe" -> 0

    // Загрузить из файла. true = успех.
    bool loadFromFile(const std::string& path);

    // Найти данные предмета по id. nullptr = не найден.
    const ItemData* find(const std::string& id) const;
};