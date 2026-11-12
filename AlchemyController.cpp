#include "AlchemyController.hpp"
#include "UIUtils.hpp"
#include <cmath>

bool AlchemyController::handleCauldronClick(float mx, float my,
                                           AlchemySystem& alchemy,
                                           Inventory& inv,
                                           SoundManager& sound,
                                           const ItemDatabase& /*itemDB*/,
                                           std::vector<FloatingText>& floatingTexts)
{
    if (!alchemy.isCauldronOpen) return false;

    float winX = 60.f;
    float winY = 60.f;

    // Если открыта книга рецептов: пагинация, клик по [X] или вне закрывает её
    if (alchemy.showRecipes)
    {
        float bx = winX + 110.f;
        float by = winY + 35.f;
        float cx = bx + 460.f - 36.f;
        float cy = by + 12.f;

        // Кнопка [ < ] предыдущая страница
        if (isInsideRect(mx, my, bx + 140.f, by + 368.f, 34.f, 26.f))
        {
            if (alchemy.recipePage > 0)
            {
                alchemy.recipePage--;
                sound.playClick();
            }
            return true;
        }

        // Кнопка [ > ] следующая страница
        if (isInsideRect(mx, my, bx + 286.f, by + 368.f, 34.f, 26.f))
        {
            int discoveredCount = 0;
            for (const auto& r : alchemy.getRecipes()) { if (r.discovered) discoveredCount++; }
            int totalPages = std::max(1, (discoveredCount + 2) / 3);
            if (alchemy.recipePage < totalPages - 1)
            {
                alchemy.recipePage++;
                sound.playClick();
            }
            return true;
        }

        if (isInside(mx, my, cx, cy, 26.f) || !isInsideRect(mx, my, bx, by, 460.f, 410.f))
        {
            alchemy.showRecipes = false;
            return true;
        }
        return true;
    }

    // Кнопка [X] закрытия котла
    if (isInside(mx, my, winX + 735.f, winY + 20.f, 32.f))
    {
        alchemy.isCauldronOpen = false;
        alchemy.showRecipes = false;
        return true;
    }

    // Кнопка "Рецепты [R]"
    float recBtnX = winX + 510.f;
    float recBtnY = winY + 16.f;
    if (isInsideRect(mx, my, recBtnX, recBtnY, 145.f, 34.f))
    {
        alchemy.showRecipes = !alchemy.showRecipes;
        return true;
    }

    // Кнопка "Сварить"
    float btnX = winX + 475.f;
    float btnY = winY + 400.f;
    float btnW = 160.f;
    float btnH = 42.f;
    if (isInsideRect(mx, my, btnX, btnY, btnW, btnH) && !alchemy.isBrewing)
    {
        std::string err;
        if (!alchemy.tryStartBrewing(err))
        {
            FloatingText ft;
            ft.text = err;
            ft.position = { winX + 430.f, winY + 360.f };
            ft.color = sf::Color(255, 120, 120);
            ft.timer = 1.5f;
            floatingTexts.push_back(ft);
        }
        else
        {
            sound.playBrew();
        }
        return true;
    }

    // Центральный слот базы
    float cauldronCenterX = winX + 555.f;
    float cauldronCenterY = winY + 235.f;
    float baseSlotX = cauldronCenterX - 20.f;
    float baseSlotY = cauldronCenterY - 10.f;
    if (isInside(mx, my, baseSlotX, baseSlotY, 40.f))
    {
        if (!alchemy.cauldronBase.isEmpty())
        {
            inv.addItem(alchemy.cauldronBase.id, alchemy.cauldronBase.count, 99);
            alchemy.cauldronBase = Item{};
            sound.playPop();
        }
        return true;
    }

    // 6 слотов ингредиентов вокруг котла
    float radius = 85.f;
    for (int i = 0; i < 6; i++)
    {
        float angle = i * (3.14159265f / 3.f) - 3.14159265f / 2.f;
        float sx = cauldronCenterX + std::cos(angle) * radius - 20.f;
        float sy = cauldronCenterY + std::sin(angle) * radius - 20.f;

        if (isInside(mx, my, sx, sy, 40.f))
        {
            if (!alchemy.cauldronIngredients[i].isEmpty())
            {
                inv.addItem(alchemy.cauldronIngredients[i].id, alchemy.cauldronIngredients[i].count, 99);
                alchemy.cauldronIngredients[i] = Item{};
                sound.playPop();
            }
            return true;
        }
    }

    // Слоты инвентаря слева
    float invX = winX + 20.f;
    float invY = winY + 80.f;
    float slotSize = 40.f;
    float slotPad = 4.f;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float sx = invX + col * (slotSize + slotPad);
            float sy = invY + row * (slotSize + slotPad);
            if (isInside(mx, my, sx, sy, slotSize))
            {
                int slotIdx = (row == 3) ? col : (9 + row * 9 + col);
                const Item& item = inv.getSlot(slotIdx);
                if (!item.isEmpty())
                {
                    if (item.id == "empty_flask" || item.id == "water_flask")
                    {
                        if (alchemy.cauldronBase.isEmpty())
                        {
                            alchemy.cauldronBase = Item{ item.id, 1 };
                            inv.removeItem(item.id, 1);
                            sound.playPop();
                        }
                    }
                    else
                    {
                        bool added = false;
                        for (int i = 0; i < 6; i++)
                        {
                            if (!alchemy.cauldronIngredients[i].isEmpty() &&
                                alchemy.cauldronIngredients[i].id == item.id &&
                                alchemy.cauldronIngredients[i].count < 99)
                            {
                                alchemy.cauldronIngredients[i].count++;
                                inv.removeItem(item.id, 1);
                                sound.playPop();
                                added = true;
                                break;
                            }
                        }
                        if (!added)
                        {
                            for (int i = 0; i < 6; i++)
                            {
                                if (alchemy.cauldronIngredients[i].isEmpty())
                                {
                                    alchemy.cauldronIngredients[i] = Item{ item.id, 1 };
                                    inv.removeItem(item.id, 1);
                                    sound.playPop();
                                    added = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                return true;
            }
        }
    }

    return true;
}

bool AlchemyController::handleTableClick(float mx, float my,
                                        AlchemySystem& alchemy,
                                        Inventory& inv,
                                        SoundManager& sound,
                                        const ItemDatabase& /*itemDB*/,
                                        std::vector<FloatingText>& floatingTexts)
{
    if (!alchemy.isTableOpen) return false;

    float winX = 60.f;
    float winY = 60.f;

    // Кнопка [X]
    if (isInside(mx, my, winX + 735.f, winY + 20.f, 32.f))
    {
        alchemy.isTableOpen = false;
        return true;
    }

    float rightCenterX = winX + 600.f;
    float inX = rightCenterX - 110.f;
    float inY = winY + 180.f;
    float outX = rightCenterX + 60.f;
    float outY = winY + 180.f;

    // Забрать из входного слота
    if (isInside(mx, my, inX, inY, 50.f))
    {
        if (!alchemy.mortarInput.isEmpty())
        {
            inv.addItem(alchemy.mortarInput.id, alchemy.mortarInput.count, 99);
            alchemy.mortarInput = Item{};
        }
        return true;
    }

    // Забрать из выходного слота
    if (isInside(mx, my, outX, outY, 50.f))
    {
        if (!alchemy.mortarOutput.isEmpty())
        {
            inv.addItem(alchemy.mortarOutput.id, alchemy.mortarOutput.count, 99);
            alchemy.mortarOutput = Item{};
        }
        return true;
    }

    // Кнопка "Перетереть"
    float btnX = rightCenterX - 90.f;
    float btnY = winY + 330.f;
    float btnW = 180.f;
    float btnH = 44.f;
    if (isInsideRect(mx, my, btnX, btnY, btnW, btnH))
    {
        if (alchemy.isGrinding) return true;

        if (alchemy.mortarInput.isEmpty())
        {
            FloatingText ft;
            ft.text = "Положите сырьё в ячейку слева!";
            ft.position = { btnX - 10.f, btnY - 30.f };
            ft.color = sf::Color(255, 120, 120);
            ft.timer = 1.5f;
            floatingTexts.push_back(ft);
            return true;
        }

        std::string resId = alchemy.getGrindResultId();
        if (resId.empty())
        {
            FloatingText ft;
            ft.text = "Этот предмет нельзя перетереть!";
            ft.position = { btnX - 10.f, btnY - 30.f };
            ft.color = sf::Color(255, 120, 120);
            ft.timer = 1.5f;
            floatingTexts.push_back(ft);
            return true;
        }

        if (!alchemy.mortarOutput.isEmpty())
        {
            if (alchemy.mortarOutput.id != resId)
            {
                FloatingText ft;
                ft.text = "Заберите другой порошок из ячейки!";
                ft.position = { btnX - 25.f, btnY - 30.f };
                ft.color = sf::Color(255, 120, 120);
                ft.timer = 1.8f;
                floatingTexts.push_back(ft);
                sound.playPop();
                return true;
            }
            if (alchemy.mortarOutput.count >= 99)
            {
                FloatingText ft;
                ft.text = "Ячейка порошка переполнена!";
                ft.position = { btnX - 10.f, btnY - 30.f };
                ft.color = sf::Color(255, 120, 120);
                ft.timer = 1.8f;
                floatingTexts.push_back(ft);
                return true;
            }
        }

        alchemy.startGrinding();
        return true;
    }

    // Слоты инвентаря слева
    float invX = winX + 25.f;
    float invY = winY + 90.f;
    float slotSize = 40.f;
    float slotPad = 4.f;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            float sx = invX + col * (slotSize + slotPad);
            float sy = invY + row * (slotSize + slotPad);
            if (isInside(mx, my, sx, sy, slotSize))
            {
                int slotIdx = (row == 3) ? col : (9 + row * 9 + col);
                const Item& item = inv.getSlot(slotIdx);
                if (!item.isEmpty())
                {
                    if (item.id == "berry" || item.id == "leaves" || item.id == "fiber" || item.id == "chamomile")
                    {
                        if (alchemy.mortarInput.isEmpty())
                        {
                            alchemy.mortarInput = Item{ item.id, 1 };
                            inv.removeItem(item.id, 1);
                        }
                        else if (alchemy.mortarInput.id == item.id && alchemy.mortarInput.count < 99)
                        {
                            alchemy.mortarInput.count++;
                            inv.removeItem(item.id, 1);
                        }
                    }
                }
                return true;
            }
        }
    }

    return true;
}

bool AlchemyController::handleRightClick(float mx, float my,
                                        const AlchemySystem& alchemy,
                                        const Inventory& inv,
                                        std::string& inspectedItemId)
{
    float winX = 60.f;
    float winY = 60.f;

    // 1. Проверяем клик ПКМ по предмету в котле
    if (alchemy.isCauldronOpen)
    {
        float invX = winX + 20.f;
        float invY = winY + 80.f;
        float slotSize = 40.f;
        float slotPad = 4.f;

        for (int row = 0; row < 4; row++)
        {
            for (int col = 0; col < 9; col++)
            {
                float sx = invX + col * (slotSize + slotPad);
                float sy = invY + row * (slotSize + slotPad);
                if (isInside(mx, my, sx, sy, slotSize))
                {
                    int slotIdx = (row == 3) ? col : (9 + row * 9 + col);
                    const Item& sItem = inv.getSlot(slotIdx);
                    if (!sItem.isEmpty()) { inspectedItemId = sItem.id; return true; }
                }
            }
        }

        float cauldronCenterX = winX + 555.f;
        float cauldronCenterY = winY + 235.f;
        if (isInside(mx, my, cauldronCenterX - 20.f, cauldronCenterY - 10.f, 40.f) && !alchemy.cauldronBase.isEmpty())
        {
            inspectedItemId = alchemy.cauldronBase.id;
            return true;
        }

        float radius = 85.f;
        for (int i = 0; i < 6; i++)
        {
            float angle = i * (3.14159265f / 3.f) - 3.14159265f / 2.f;
            float sx = cauldronCenterX + std::cos(angle) * radius - 20.f;
            float sy = cauldronCenterY + std::sin(angle) * radius - 20.f;
            if (isInside(mx, my, sx, sy, 40.f) && !alchemy.cauldronIngredients[i].isEmpty())
            {
                inspectedItemId = alchemy.cauldronIngredients[i].id;
                return true;
            }
        }
        return true;
    }

    // 2. Проверяем клик ПКМ по предмету на столе алхимика
    if (alchemy.isTableOpen)
    {
        float inX = winX + 600.f - 110.f;
        float inY = winY + 180.f;
        float outX = winX + 600.f + 60.f;
        float outY = winY + 180.f;

        if (isInside(mx, my, inX, inY, 50.f) && !alchemy.mortarInput.isEmpty())
        {
            inspectedItemId = alchemy.mortarInput.id;
            return true;
        }
        if (isInside(mx, my, outX, outY, 50.f) && !alchemy.mortarOutput.isEmpty())
        {
            inspectedItemId = alchemy.mortarOutput.id;
            return true;
        }

        float invX = winX + 25.f;
        float invY = winY + 90.f;
        float slotSize = 40.f;
        float slotPad = 4.f;
        for (int row = 0; row < 4; row++)
        {
            for (int col = 0; col < 9; col++)
            {
                float sx = invX + col * (slotSize + slotPad);
                float sy = invY + row * (slotSize + slotPad);
                if (isInside(mx, my, sx, sy, slotSize))
                {
                    int slotIdx = (row == 3) ? col : (9 + row * 9 + col);
                    const Item& sItem = inv.getSlot(slotIdx);
                    if (!sItem.isEmpty()) { inspectedItemId = sItem.id; return true; }
                }
            }
        }
        return true;
    }

    return false;
}
