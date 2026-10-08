#include "Inventory.hpp"

// ============================================
// ДОБАВИТЬ ПРЕДМЕТ
// ============================================
bool Inventory::addItem(const std::string& id, int count, int maxStack)
{
    if (id.empty() || count <= 0) return false;
    if (maxStack <= 0) maxStack = 1;

    // 1. Сначала ищем стеки с тем же id, где есть место
    for (auto& slot : slots)
    {
        if (slot.id == id && slot.count < maxStack)
        {
            int space = maxStack - slot.count;
            int toAdd = (count < space) ? count : space;
            slot.count += toAdd;
            count -= toAdd;
            if (count <= 0) return true;
        }
    }

    // 2. Потом ищем пустые слоты
    for (auto& slot : slots)
    {
        if (slot.isEmpty())
        {
            int toAdd = (count < maxStack) ? count : maxStack;
            slot.id = id;
            slot.count = toAdd;
            count -= toAdd;
            if (count <= 0) return true;
        }
    }

    // 3. Если осталось — вернуть false (не уместилось)
    return count <= 0;
}

// ============================================
// УБРАТЬ ПРЕДМЕТ
// ============================================
bool Inventory::removeItem(const std::string& id, int count)
{
    if (id.empty() || count <= 0) return false;

    // Проверяем, хватает ли
    if (countItem(id) < count) return false;

    // Убираем
    for (auto& slot : slots)
    {
        if (slot.id == id && slot.count > 0)
        {
            int toRemove = (count < slot.count) ? count : slot.count;
            slot.count -= toRemove;
            count -= toRemove;

            if (slot.count <= 0)
            {
                slot.id.clear();
                slot.count = 0;
            }

            if (count <= 0) return true;
        }
    }

    return count <= 0;
}

// ============================================
// ПОСЧИТАТЬ ПРЕДМЕТ
// ============================================
int Inventory::countItem(const std::string& id) const
{
    int total = 0;
    for (const auto& slot : slots)
    {
        if (slot.id == id)
            total += slot.count;
    }
    return total;
}

// ============================================
// ДОСТУП К СЛОТУ
// ============================================
const Item& Inventory::getSlot(int index) const
{
    return slots[index];
}

// ============================================
// УСТАНОВИТЬ СЛОТ
// ============================================
void Inventory::setSlot(int index, const Item& item)
{
    slots[index] = item;
}