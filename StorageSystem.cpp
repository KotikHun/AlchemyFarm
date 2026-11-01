#include "StorageSystem.hpp"
#include <algorithm>

void StorageSystem::transferToChest(Inventory& srcInv,
                                    int srcSlot,
                                    Chest& chest,
                                    const ItemDatabase& itemDB,
                                    std::vector<FloatingText>& floatingTexts,
                                    sf::Vector2f playerPos)
{
    Item src = srcInv.getSlot(srcSlot);
    if (src.isEmpty()) return;

    if (chest.type == "shipping_bin")
    {
        const ItemData* itmData = itemDB.find(src.id);
        if (src.id == "hoe" || src.id == "watering_can" || src.id == "axe" || src.id == "pickaxe" ||
            (itmData && (itmData->isTool || itmData->sellPrice <= 0)))
        {
            FloatingText ft;
            ft.text = "Инструменты нельзя продавать! Они нужны для работы.";
            ft.position = playerPos;
            ft.color = sf::Color(255, 120, 120);
            ft.timer = 1.8f;
            floatingTexts.push_back(ft);
            return;
        }
    }

    int maxStack = 99;
    const ItemData* d = itemDB.find(src.id);
    if (d && d->maxStack > 0) maxStack = d->maxStack;

    // 1. Ищем существующие стаки
    for (int i = 0; i < chest.slotCount; i++)
    {
        Item& slot = chest.inventory.slots[i];
        if (slot.id == src.id && slot.count < maxStack)
        {
            int space = maxStack - slot.count;
            int toMove = std::min(src.count, space);
            slot.count += toMove;
            src.count -= toMove;
            if (src.count <= 0)
            {
                srcInv.setSlot(srcSlot, Item{});
                return;
            }
        }
    }
    // 2. Ищем пустые слоты
    for (int i = 0; i < chest.slotCount; i++)
    {
        Item& slot = chest.inventory.slots[i];
        if (slot.isEmpty())
        {
            slot = src;
            srcInv.setSlot(srcSlot, Item{});
            return;
        }
    }
    srcInv.setSlot(srcSlot, src);
}

void StorageSystem::transferToPlayer(Chest& chest, int chestSlot, Inventory& playerInv, const ItemDatabase& itemDB)
{
    Item src = chest.inventory.getSlot(chestSlot);
    if (src.isEmpty()) return;
    int maxStack = 99;
    const ItemData* d = itemDB.find(src.id);
    if (d && d->maxStack > 0) maxStack = d->maxStack;

    // 1. Ищем существующие стаки
    for (int i = 0; i < Inventory::SIZE; i++)
    {
        Item& slot = playerInv.slots[i];
        if (slot.id == src.id && slot.count < maxStack)
        {
            int space = maxStack - slot.count;
            int toMove = std::min(src.count, space);
            slot.count += toMove;
            src.count -= toMove;
            if (src.count <= 0)
            {
                chest.inventory.setSlot(chestSlot, Item{});
                return;
            }
        }
    }
    // 2. Ищем пустые слоты
    for (int i = 0; i < Inventory::SIZE; i++)
    {
        Item& slot = playerInv.slots[i];
        if (slot.isEmpty())
        {
            slot = src;
            chest.inventory.setSlot(chestSlot, Item{});
            return;
        }
    }
    chest.inventory.setSlot(chestSlot, src);
}
