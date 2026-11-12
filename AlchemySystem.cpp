#include "AlchemySystem.hpp"

AlchemySystem::AlchemySystem()
{
    // =========================================================================
    // 1. РОМАШКОВАЯ ЛИНЕЙКА (Защита от бессонницы + Регенерация)
    // =========================================================================
    // 1.1 Ромашковый отвар (Слабая насыщенность) — сырье
    {
        PotionRecipe r;
        r.resultId = "chamomile_decoction";
        r.name = "Ромашковый отвар";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"chamomile", 2} };
        r.description = "Легкий золотистый отвар из цветков ромашки в изящной колбочке. Защищает от бессонной ночи.";
        r.effectText = "100% энергии утром даже при обмороке (+20 сейчас)";
        r.formulaText = "2 Ромашки";
        r.discovered = true;
        recipes.push_back(r);
    }
    // 1.2 Ромашковый отвар (Слабая насыщенность) — из пыльцы
    {
        PotionRecipe r;
        r.resultId = "chamomile_decoction";
        r.name = "Ромашковый отвар (из пыльцы)";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"chamomile_powder", 1} };
        r.description = "Быстрый отвар из тонко перетертой ромашковой пыльцы.";
        r.effectText = "100% энергии утром даже при обмороке (+20 сейчас)";
        r.formulaText = "1 Ромашковая пыльца";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 1.3 Густой ромашковый чай (Средняя насыщенность) — сырье
    {
        PotionRecipe r;
        r.resultId = "chamomile_tea";
        r.name = "Густой ромашковый чай";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"chamomile", 4} };
        r.description = "Янтарно-медовый настой ромашки в трапециевидной колбе. Дарит глубокий отдых и восстанавливает дыхание.";
        r.effectText = "100% энергии утром + регенерация (+1/сек на 30 сек)";
        r.formulaText = "4 Ромашки";
        r.discovered = false;
        recipes.push_back(r);
    }
    // 1.4 Густой ромашковый чай (Средняя насыщенность) — из пыльцы
    {
        PotionRecipe r;
        r.resultId = "chamomile_tea";
        r.name = "Густой ромашковый чай (из пыльцы)";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"chamomile_powder", 2} };
        r.description = "Концентрированный чай на ромашковой пыльце.";
        r.effectText = "100% энергии утром + регенерация (+1/сек на 30 сек)";
        r.formulaText = "2 Ромашковой пыльцы";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 1.5 Истинная настойка ромашки (Максимальная насыщенность) — только порошок
    {
        PotionRecipe r;
        r.resultId = "chamomile_tincture";
        r.name = "Истинная настойка ромашки";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"chamomile_powder", 3} };
        r.description = "Изумрудный концентрат максимальной насыщенности из ромашковой пыльцы в массивном флаконе.";
        r.effectText = "100% энергии утром + мощная регенерация (+3/сек на 30 сек)";
        r.formulaText = "3 Ромашковой пыльцы";
        r.discovered = false;
        recipes.push_back(r);
    }

    // =========================================================================
    // 2. ЯГОДНАЯ ЛИНЕЙКА (Мгновенные всплески бодрости)
    // =========================================================================
    // 2.1 Ягодный морс (Слабая насыщенность) — сырье
    {
        PotionRecipe r;
        r.resultId = "berry_juice";
        r.name = "Ягодный морс";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry", 2} };
        r.description = "Освежающий кисло-сладкий напиток из лесных ягод в маленькой колбе.";
        r.effectText = "+35 энергии мгновенно";
        r.formulaText = "2 Ягоды";
        r.discovered = true;
        recipes.push_back(r);
    }
    // 2.2 Ягодный морс (Слабая насыщенность) — из порошка
    {
        PotionRecipe r;
        r.resultId = "berry_juice";
        r.name = "Ягодный морс (из порошка)";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry_powder", 1} };
        r.description = "Быстрый ягодный морс из перетертых ягод.";
        r.effectText = "+35 энергии мгновенно";
        r.formulaText = "1 Ягодный порошок";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 2.3 Спелый ягодный взвар (Средняя насыщенность) — сырье
    {
        PotionRecipe r;
        r.resultId = "berry_decoction";
        r.name = "Спелый ягодный взвар";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry", 4} };
        r.description = "Насыщенный рубиновый взвар из спелых ягод в конической колбе. Быстро возвращает силы.";
        r.effectText = "+70 энергии мгновенно";
        r.formulaText = "4 Ягоды";
        r.discovered = false;
        recipes.push_back(r);
    }
    // 2.4 Спелый ягодный взвар (Средняя насыщенность) — из порошка
    {
        PotionRecipe r;
        r.resultId = "berry_decoction";
        r.name = "Спелый ягодный взвар (из порошка)";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry_powder", 2} };
        r.description = "Густой ягодный взвар из двух мер ягодного порошка.";
        r.effectText = "+70 энергии мгновенно";
        r.formulaText = "2 Ягодных порошка";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 2.5 Концентрированный ягодный эликсир (Максимальная насыщенность) — только порошок
    {
        PotionRecipe r;
        r.resultId = "berry_elixir";
        r.name = "Концентрированный ягодный эликсир";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry_powder", 3} };
        r.description = "Густой фиолетово-малиновый эликсир предельной насыщенности в тяжелом флаконе.";
        r.effectText = "+110 энергии мгновенно";
        r.formulaText = "3 Ягодных порошка";
        r.discovered = false;
        recipes.push_back(r);
    }

    // =========================================================================
    // 3. КОМБИНИРОВАННАЯ ЛИНЕЙКА (Ягода + Ромашка)
    // =========================================================================
    // 3.1 Лесной сбор (Слабая насыщенность)
    {
        PotionRecipe r;
        r.resultId = "forest_infusion";
        r.name = "Лесной сбор";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry", 1}, {"chamomile", 1} };
        r.description = "Гармоничный настой из лесных ягод и ромашки в компактной колбе.";
        r.effectText = "+40 энергии + 100% энергии на следующее утро";
        r.formulaText = "1 Ягода + 1 Ромашка";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 3.2 Тонизирующий настой (Средняя насыщенность) — сырье
    {
        PotionRecipe r;
        r.resultId = "tonic_infusion";
        r.name = "Тонизирующий настой";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry", 2}, {"chamomile", 2} };
        r.description = "Благородный сиреневый настой в колбе-трапеции. Бодрит тело и защищает от усталости.";
        r.effectText = "+80 энергии + регенерация (+2/сек) + 100% энергии утром";
        r.formulaText = "2 Ягоды + 2 Ромашки";
        r.discovered = false;
        recipes.push_back(r);
    }
    // 3.3 Тонизирующий настой (Средняя насыщенность) — порошки
    {
        PotionRecipe r;
        r.resultId = "tonic_infusion";
        r.name = "Тонизирующий настой (из порошков)";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry_powder", 1}, {"chamomile_powder", 1} };
        r.description = "Тонизирующий настой на тонких порошках ягод и ромашки.";
        r.effectText = "+80 энергии + регенерация (+2/сек) + 100% энергии утром";
        r.formulaText = "1 Ягодный порошок + 1 Ромашковая пыльца";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 3.4 Великий алхимический нектар (Максимальная насыщенность) — порошки
    {
        PotionRecipe r;
        r.resultId = "grand_nectar";
        r.name = "Великий алхимический нектар";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry_powder", 2}, {"chamomile_powder", 2} };
        r.description = "Шедевр алхимии из двух порошков высшей очистки. Густой сияющий нектар в квадратном флаконе.";
        r.effectText = "+120 энергии + регенерация (+3/сек) + скорость +40% + 100% утром";
        r.formulaText = "2 Ягодных порошка + 2 Ромашковой пыльцы";
        r.discovered = false;
        recipes.push_back(r);
    }
}

const PotionRecipe* AlchemySystem::findMatchingRecipe() const
{
    if (cauldronBase.isEmpty()) return nullptr;

    // Считаем ингредиенты в котле
    std::map<std::string, int> counts;
    for (const auto& itm : cauldronIngredients)
    {
        if (!itm.isEmpty())
        {
            counts[itm.id] += itm.count;
        }
    }

    if (counts.empty()) return nullptr;

    // Ищем точное совпадение
    for (const auto& r : recipes)
    {
        if (r.baseId == cauldronBase.id && r.requiredIngredients == counts)
        {
            return &r;
        }
    }

    return nullptr;
}

bool AlchemySystem::tryStartBrewing(std::string& outError)
{
    if (isBrewing) return false;

    if (cauldronBase.id != "water_flask")
    {
        outError = "В центре котла должен быть флакон с чистой водой!";
        return false;
    }

    // Проверяем, есть ли хоть какие-то ингредиенты в ячейках
    bool hasAny = false;
    for (const auto& itm : cauldronIngredients)
    {
        if (!itm.isEmpty())
        {
            hasAny = true;
            break;
        }
    }

    if (!hasAny)
    {
        outError = "Положите ингредиенты в ячейки вокруг котла!";
        return false;
    }

    isBrewing = true;
    brewTimer = 0.f;
    return true;
}

BrewResult AlchemySystem::finishBrewing()
{
    isBrewing = false;
    brewTimer = 0.f;

    const PotionRecipe* matched = findMatchingRecipe();

    // Очищаем ингредиенты в ячейках
    for (auto& itm : cauldronIngredients)
    {
        itm = Item{};
    }

    BrewResult res;
    if (matched != nullptr)
    {
        // Успешная варка!
        res.success = true;
        res.resultId = matched->resultId;
        res.resultName = matched->name;
        res.message = "+1 " + matched->name + " сварено!";
        cauldronBase = Item{ matched->resultId, 1 };

        // Разблокируем рецепт
        unlockRecipe(matched->name);
    }
    else
    {
        // Рецепт не совпал: вода испорчена, ингредиенты сгорели
        res.success = false;
        res.resultId = "";
        res.message = "Варка не удалась! Неизвестный рецепт (смесь сгорела).";
        cauldronBase = Item{ "empty_flask", 1 }; // Остался только пустой флакон
    }

    return res;
}

bool AlchemySystem::canGrind() const
{
    if (mortarInput.isEmpty()) return false;
    std::string resId = getGrindResultId();
    if (resId.empty()) return false;

    // Выходная ячейка должна быть свободна или содержать тот же порошок (не более 99)
    if (!mortarOutput.isEmpty())
    {
        if (mortarOutput.id != resId || mortarOutput.count >= 99)
            return false;
    }
    return true;
}

std::string AlchemySystem::getGrindResultId() const
{
    if (mortarInput.id == "berry") return "berry_powder";
    if (mortarInput.id == "leaves" || mortarInput.id == "fiber") return "herbal_powder";
    if (mortarInput.id == "chamomile") return "chamomile_powder";
    return "";
}

void AlchemySystem::startGrinding()
{
    if (!canGrind() || isGrinding) return;
    isGrinding = true;
    grindTimer = 0.f;
}

bool AlchemySystem::finishGrinding()
{
    if (!isGrinding) return false;
    isGrinding = false;
    grindTimer = 0.f;

    std::string resId = getGrindResultId();
    if (resId.empty()) return false;

    // Защита: если выходная ячейка занята другим предметом или переполнена, не тратим сырьё
    if (!mortarOutput.isEmpty() && (mortarOutput.id != resId || mortarOutput.count >= 99))
    {
        return false;
    }

    // Тратим 1 входной предмет
    mortarInput.count--;
    if (mortarInput.count <= 0)
    {
        mortarInput = Item{};
    }

    // Добавляем в выход (новый предмет или увеличиваем стак)
    if (mortarOutput.isEmpty())
    {
        mortarOutput = Item{ resId, 1 };
    }
    else if (mortarOutput.id == resId)
    {
        mortarOutput.count++;
    }
    return true;
}

void AlchemySystem::unlockRecipe(const std::string& recipeIdentifier)
{
    for (auto& r : recipes)
    {
        if (r.name == recipeIdentifier || r.resultId == recipeIdentifier)
        {
            r.discovered = true;
        }
    }
}
