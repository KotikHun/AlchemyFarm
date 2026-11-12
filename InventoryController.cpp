#include "InventoryController.hpp"
#include "StorageSystem.hpp"
#include "UIUtils.hpp"

int InventoryController::getChestSlotUnderMouse(sf::Vector2i mousePos, const Chest* nearbyChest)
{
    bool chestOpen = (nearbyChest != nullptr && nearbyChest->isOpen);
    if (!chestOpen || nearbyChest == nullptr) return -1;

    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    const float GRID_X = 235.f;
    const float GRID_Y = 165.f;
    int rows = nearbyChest->slotCount / 9;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float x = GRID_X + col * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            float y = GRID_Y + row * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            if (isInside(mx, my, x, y, UI::SLOT_SIZE))
            {
                return row * 9 + col;
            }
        }
    }
    return -1;
}

int InventoryController::getSlotUnderMouse(sf::Vector2i mousePos, bool chestOpen)
{
    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    const float GRID_X = 235.f;
    float GRID_Y = chestOpen ? 340.f : 220.f;

    // Слоты 9-35 (рюкзак)
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float x = GRID_X + col * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            float y = GRID_Y + row * (UI::SLOT_SIZE + UI::SLOT_PADDING);
            if (isInside(mx, my, x, y, UI::SLOT_SIZE))
            {
                return 9 + row * 9 + col;
            }
        }
    }

    // Хотбар 0-8
    float HOTBAR_Y = GRID_Y + 3 * (UI::SLOT_SIZE + UI::SLOT_PADDING) + 10.f;
    for (int col = 0; col < 9; col++)
    {
        float x = GRID_X + col * (UI::SLOT_SIZE + UI::SLOT_PADDING);
        if (isInside(mx, my, x, HOTBAR_Y, UI::SLOT_SIZE))
        {
            return col;
        }
    }

    return -1;
}

bool InventoryController::handleTabClicks(float mx, float my,
                                          InventoryTab& currentTab,
                                          int& journalCategory,
                                          SoundManager& sound)
{
    // Клик по вкладкам сверху (Рюкзак, Крафт, Дневник)
    if (my >= 175.f && my <= 207.f && mx >= 225.f && mx <= 540.f)
    {
        int tabIdx = static_cast<int>((mx - 225.f) / 105.f);
        if (tabIdx >= 0 && tabIdx < 3)
        {
            sound.playClick();
            currentTab = static_cast<InventoryTab>(tabIdx);
            return true;
        }
    }

    // Вкладка Дневника (выбор категорий справа: Задания, Зелья, Растения, Ремесло)
    if (currentTab == InventoryTab::Journal)
    {
        float tabX = 235.f + 350.f + 10.f; // 595.f
        if (mx >= tabX && mx <= tabX + 120.f)
        {
            for (int c = 0; c < 4; c++)
            {
                float ty = 220.f + 12.f + c * 38.f;
                if (my >= ty && my <= ty + 30.f)
                {
                    journalCategory = c;
                    sound.playClick();
                    return true;
                }
            }
        }
        return true;
    }

    return false;
}

bool InventoryController::handleCraftingClick(float mx, float my,
                                             Inventory& inv,
                                             Player& player,
                                             SoundManager& sound,
                                             std::vector<FloatingText>& floatingTexts)
{
    for (int r = 0; r < 7; r++)
    {
        sf::FloatRect rect = UI::getCraftingRecipeRect(r);
        if (rect.contains({ mx, my }))
        {
            sf::Vector2f ppos = player.shape.getPosition();
            if (r == 0) // Сундук
            {
                if (inv.countItem("wood") >= 20)
                {
                    inv.removeItem("wood", 20);
                    inv.addItem("chest", 1, 99);
                    sound.playPop();
                    FloatingText ft;
                    ft.text = "+1 Сундук создан!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 220, 100);
                    ft.timer = 1.5f;
                    floatingTexts.push_back(ft);
                }
                else
                {
                    FloatingText ft;
                    ft.text = "Нужно 20 Древесины!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 100, 100);
                    ft.timer = 1.3f;
                    floatingTexts.push_back(ft);
                }
            }
            else if (r == 1) // Стол алхимика
            {
                if (inv.countItem("wood") >= 15 && inv.countItem("stone") >= 10)
                {
                    inv.removeItem("wood", 15);
                    inv.removeItem("stone", 10);
                    inv.addItem("alchemist_table", 1, 99);
                    sound.playPop();
                    FloatingText ft;
                    ft.text = "+1 Стол алхимика создан!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 215, 110);
                    ft.timer = 1.5f;
                    floatingTexts.push_back(ft);
                }
                else
                {
                    FloatingText ft;
                    ft.text = "Нужно 15 Дерева и 10 Камня!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 100, 100);
                    ft.timer = 1.3f;
                    floatingTexts.push_back(ft);
                }
            }
            else if (r == 2) // Алхимический котел
            {
                if (inv.countItem("stone") >= 15 && inv.countItem("wood") >= 5)
                {
                    inv.removeItem("stone", 15);
                    inv.removeItem("wood", 5);
                    inv.addItem("cauldron", 1, 99);
                    sound.playPop();
                    FloatingText ft;
                    ft.text = "+1 Алхимический котел создан!";
                    ft.position = ppos;
                    ft.color = sf::Color(140, 255, 180);
                    ft.timer = 1.5f;
                    floatingTexts.push_back(ft);
                }
                else
                {
                    FloatingText ft;
                    ft.text = "Нужно 15 Камня и 5 Дерева!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 100, 100);
                    ft.timer = 1.3f;
                    floatingTexts.push_back(ft);
                }
            }
            else if (r == 3) // Уютная кровать
            {
                if (inv.countItem("wood") >= 25 && inv.countItem("fiber") >= 10)
                {
                    inv.removeItem("wood", 25);
                    inv.removeItem("fiber", 10);
                    inv.addItem("bed", 1, 99);
                    sound.playPop();
                    FloatingText ft;
                    ft.text = "+1 Кровать создана!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 200, 140);
                    ft.timer = 1.5f;
                    floatingTexts.push_back(ft);
                }
                else
                {
                    FloatingText ft;
                    ft.text = "Нужно 25 Дерева и 10 Волокна!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 100, 100);
                    ft.timer = 1.3f;
                    floatingTexts.push_back(ft);
                }
            }
            else if (r == 4) // Мягкий ковер
            {
                if (inv.countItem("fiber") >= 12)
                {
                    inv.removeItem("fiber", 12);
                    inv.addItem("carpet", 1, 99);
                    sound.playPop();
                    FloatingText ft;
                    ft.text = "+1 Ковер создан!";
                    ft.position = ppos;
                    ft.color = sf::Color(200, 180, 255);
                    ft.timer = 1.5f;
                    floatingTexts.push_back(ft);
                }
                else
                {
                    FloatingText ft;
                    ft.text = "Нужно 12 Волокна!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 100, 100);
                    ft.timer = 1.3f;
                    floatingTexts.push_back(ft);
                }
            }
            else if (r == 5) // Пустой флакон
            {
                if (inv.countItem("stone") >= 1)
                {
                    inv.removeItem("stone", 1);
                    inv.addItem("empty_flask", 1, 99);
                    sound.playPop();
                    FloatingText ft;
                    ft.text = "+1 Пустой флакон создан!";
                    ft.position = ppos;
                    ft.color = sf::Color(180, 240, 255);
                    ft.timer = 1.3f;
                    floatingTexts.push_back(ft);
                }
                else
                {
                    FloatingText ft;
                    ft.text = "Нужен 1 Камень!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 100, 100);
                    ft.timer = 1.3f;
                    floatingTexts.push_back(ft);
                }
            }
            else if (r == 6) // Семена
            {
                if (inv.countItem("berry") >= 1)
                {
                    inv.removeItem("berry", 1);
                    inv.addItem("seeds", 2, 99);
                    sound.playPop();
                    FloatingText ft;
                    ft.text = "+2 Семени получено!";
                    ft.position = ppos;
                    ft.color = sf::Color(160, 255, 160);
                    ft.timer = 1.3f;
                    floatingTexts.push_back(ft);
                }
                else
                {
                    FloatingText ft;
                    ft.text = "Нужна 1 Ягода!";
                    ft.position = ppos;
                    ft.color = sf::Color(255, 100, 100);
                    ft.timer = 1.3f;
                    floatingTexts.push_back(ft);
                }
            }
            return true;
        }
    }
    return false;
}

bool InventoryController::handleDragStart(float mx, float my,
                                          sf::Vector2i mousePixel,
                                          bool isShift,
                                          bool isInventoryOpen,
                                          Chest* nearbyChest,
                                          Inventory& inv,
                                          const ItemDatabase& itemDB,
                                          std::vector<FloatingText>& floatingTexts,
                                          sf::Vector2f playerPos,
                                          bool& isDragging,
                                          Item& draggedItem,
                                          int& draggedFromSlot,
                                          bool& draggedFromChest,
                                          bool& draggedFromHotbar)
{
    bool chestOpen = (nearbyChest != nullptr && nearbyChest->isOpen);
    if (!isInventoryOpen && !chestOpen) return false;

    if (chestOpen && nearbyChest != nullptr)
    {
        int cslot = getChestSlotUnderMouse(mousePixel, nearbyChest);
        if (cslot != -1)
        {
            if (isShift)
            {
                StorageSystem::transferToPlayer(*nearbyChest, cslot, inv, itemDB);
                return true;
            }
            const Item& itm = nearbyChest->inventory.getSlot(cslot);
            if (!itm.isEmpty())
            {
                isDragging = true;
                draggedItem = itm;
                draggedFromSlot = cslot;
                draggedFromChest = true;
                nearbyChest->inventory.setSlot(cslot, Item{});
                return true;
            }
        }
    }

    int slot = getSlotUnderMouse(mousePixel, chestOpen);
    if (slot != -1)
    {
        if (chestOpen && nearbyChest != nullptr && isShift)
        {
            StorageSystem::transferToChest(inv, slot, *nearbyChest, itemDB, floatingTexts, playerPos);
            return true;
        }
        const Item& itm = inv.getSlot(slot);
        if (!itm.isEmpty())
        {
            isDragging = true;
            draggedItem = itm;
            draggedFromSlot = slot;
            draggedFromChest = false;
            draggedFromHotbar = (slot < 9);
            inv.setSlot(slot, Item{});
            return true;
        }
    }

    return false;
}

bool InventoryController::handleDragRelease(float mx, float my,
                                            sf::Vector2i mousePixel,
                                            bool& isDragging,
                                            Item& draggedItem,
                                            int& draggedFromSlot,
                                            bool& draggedFromChest,
                                            Chest* nearbyChest,
                                            Inventory& inv,
                                            const ItemDatabase& itemDB,
                                            SoundManager& sound,
                                            std::vector<FloatingText>& floatingTexts,
                                            sf::Vector2f playerPos)
{
    if (!isDragging) return false;

    bool chestOpen = (nearbyChest != nullptr && nearbyChest->isOpen);

    // 1. Корзина (удаление предмета)
    if (!chestOpen && isInside(mx, my, 665.f, 220.f + 3 * (UI::SLOT_SIZE + UI::SLOT_PADDING) + 10.f + UI::SLOT_SIZE + 10.f, UI::SLOT_SIZE))
    {
        FloatingText ft;
        ft.text = "Предмет удален";
        ft.position = playerPos;
        ft.color = sf::Color(255, 120, 120);
        ft.timer = 1.0f;
        floatingTexts.push_back(ft);

        isDragging = false;
        draggedItem = Item{};
        draggedFromSlot = -1;
        draggedFromChest = false;
        return true;
    }

    int targetChestSlot = chestOpen ? getChestSlotUnderMouse(mousePixel, nearbyChest) : -1;
    int targetInvSlot = getSlotUnderMouse(mousePixel, chestOpen);

    if (targetChestSlot != -1 && nearbyChest != nullptr)
    {
        // Дроп в слот сундука
        const Item& existing = nearbyChest->inventory.getSlot(targetChestSlot);
        if (draggedFromChest)
        {
            if (existing.isEmpty())
            {
                nearbyChest->inventory.setSlot(targetChestSlot, draggedItem);
            }
            else if (existing.id == draggedItem.id)
            {
                int newCount = existing.count + draggedItem.count;
                nearbyChest->inventory.setSlot(targetChestSlot, Item{ existing.id, newCount });
            }
            else
            {
                nearbyChest->inventory.setSlot(draggedFromSlot, existing);
                nearbyChest->inventory.setSlot(targetChestSlot, draggedItem);
            }
        }
        else
        {
            // Из инвентаря в сундук
            if (nearbyChest->type == "shipping_bin")
            {
                const ItemData* itmData = itemDB.find(draggedItem.id);
                if (draggedItem.id == "hoe" || draggedItem.id == "watering_can" || draggedItem.id == "axe" || draggedItem.id == "pickaxe" ||
                    (itmData && (itmData->isTool || itmData->sellPrice <= 0)))
                {
                    inv.setSlot(draggedFromSlot, draggedItem);
                    isDragging = false;
                    draggedItem = Item{};
                    draggedFromSlot = -1;
                    draggedFromChest = false;

                    FloatingText ft;
                    ft.text = "Инструменты нельзя продавать! Они нужны для работы.";
                    ft.position = playerPos;
                    ft.color = sf::Color(255, 120, 120);
                    ft.timer = 1.8f;
                    floatingTexts.push_back(ft);
                    sound.playPop();
                    return true;
                }
            }

            if (existing.isEmpty())
            {
                nearbyChest->inventory.setSlot(targetChestSlot, draggedItem);
            }
            else if (existing.id == draggedItem.id)
            {
                int newCount = existing.count + draggedItem.count;
                nearbyChest->inventory.setSlot(targetChestSlot, Item{ existing.id, newCount });
            }
            else
            {
                inv.setSlot(draggedFromSlot, existing);
                nearbyChest->inventory.setSlot(targetChestSlot, draggedItem);
            }
        }
    }
    else if (targetInvSlot != -1)
    {
        // Дроп в слот инвентаря игрока
        const Item& existing = inv.getSlot(targetInvSlot);
        if (draggedFromChest && nearbyChest != nullptr)
        {
            // Из сундука в инвентарь
            if (existing.isEmpty())
            {
                inv.setSlot(targetInvSlot, draggedItem);
            }
            else if (existing.id == draggedItem.id)
            {
                int newCount = existing.count + draggedItem.count;
                inv.setSlot(targetInvSlot, Item{ existing.id, newCount });
            }
            else
            {
                nearbyChest->inventory.setSlot(draggedFromSlot, existing);
                inv.setSlot(targetInvSlot, draggedItem);
            }
        }
        else
        {
            // Из инвентаря в инвентарь
            if (existing.isEmpty())
            {
                inv.setSlot(targetInvSlot, draggedItem);
            }
            else if (existing.id == draggedItem.id)
            {
                int newCount = existing.count + draggedItem.count;
                inv.setSlot(targetInvSlot, Item{ existing.id, newCount });
            }
            else
            {
                inv.setSlot(draggedFromSlot, existing);
                inv.setSlot(targetInvSlot, draggedItem);
            }
        }
    }
    else
    {
        // Возвращаем на исходное место
        if (draggedFromChest && nearbyChest != nullptr)
        {
            nearbyChest->inventory.setSlot(draggedFromSlot, draggedItem);
        }
        else
        {
            inv.setSlot(draggedFromSlot, draggedItem);
        }
    }

    isDragging = false;
    draggedItem = Item{};
    draggedFromSlot = -1;
    draggedFromChest = false;
    return true;
}
