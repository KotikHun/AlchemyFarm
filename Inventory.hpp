#pragma once

#include <array>
#include <string>
#include "Item.hpp"

enum class InventoryTab {
    Backpack,
    Crafting,
    Journal
};

// ============================================
// ИНВЕНТАРЬ ИГРОКА
// ============================================
// 36 слотов: 0-8 хотбар, 9-35 рюкзак.
// Слот = Item (id + count). Пустой = id "".

struct Inventory {
    static constexpr int SIZE = 36;
    static constexpr int HOTBAR_SIZE = 9;

    std::array<Item, SIZE> slots;

    // Добавить предмет. true = уместилось.
    // maxStack берётся из itemDB (нужен для правильного стека).
    bool addItem(const std::string& id, int count, int maxStack);

    // Убрать предмет. true = хватило.
    bool removeItem(const std::string& id, int count);

    // Сколько всего в инвентаре.
    int countItem(const std::string& id) const;

    // Получить слот (для UI и т.д.).
    const Item& getSlot(int index) const;

    // Установить слот (для drag&drop позже).
    void setSlot(int index, const Item& item);
};