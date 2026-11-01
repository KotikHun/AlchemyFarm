#include "AlchemySystem.hpp"

AlchemySystem::AlchemySystem()
{
    // === ЗЕЛЬЯ ЭНЕРГИИ ===
    // 1. Зелье энергии (базовый рецепт из ягод и волокна)
    {
        PotionRecipe r;
        r.resultId = "energy_potion";
        r.name = "Зелье энергии";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry", 1}, {"fiber", 1} };
        r.description = "Восстанавливает силы усталого фермера (+50 энергии).";
        r.effectText = "+50 энергии при употреблении (ПКМ)";
        r.formulaText = "1 Ягода + 1 Волокно";
        r.discovered = true; // Первое начальное знание алхимика
        recipes.push_back(r);
    }

    // 2. Лесной тоник (из ягод и листьев)
    {
        PotionRecipe r;
        r.resultId = "energy_potion";
        r.name = "Лесной тоник энергии";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry", 1}, {"leaves", 1} };
        r.description = "Освежающий травяной отвар из ягод и свежих древесных листьев.";
        r.effectText = "+50 энергии при употреблении (ПКМ)";
        r.formulaText = "1 Ягода + 1 Листья";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 3. Эссенция бодрости (из порошков высшей очистки)
    {
        PotionRecipe r;
        r.resultId = "energy_potion";
        r.name = "Эссенция бодрости";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry_powder", 1}, {"herbal_powder", 1} };
        r.description = "Концентрированный эликсир из тонко перетертых компонентов.";
        r.effectText = "+50 энергии при употреблении (ПКМ)";
        r.formulaText = "1 Ягодный порошок + 1 Травяной порошок";
        r.discovered = false;
        recipes.push_back(r);
    }

    // === ЗЕЛЬЯ СКОРОСТИ ===
    // 4. Зелье скорости (из листьев и семян)
    {
        PotionRecipe r;
        r.resultId = "speed_potion";
        r.name = "Зелье скорости";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"leaves", 1}, {"seeds", 1} };
        r.description = "Легкий отвар из древесных листьев и семян.";
        r.effectText = "+40% скорости бега на 30 секунд";
        r.formulaText = "1 Листья + 1 Семя";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 5. Зелье легкости (из волокна и семян)
    {
        PotionRecipe r;
        r.resultId = "speed_potion";
        r.name = "Зелье легкости";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"fiber", 1}, {"seeds", 1} };
        r.description = "Облегчающий шаг травяной отвар из сухого волокна и семян.";
        r.effectText = "+40% скорости бега на 30 секунд";
        r.formulaText = "1 Волокно + 1 Семя";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 6. Зелье стремительности (из ягод и семян)
    {
        PotionRecipe r;
        r.resultId = "speed_potion";
        r.name = "Зелье стремительности";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry", 1}, {"seeds", 2} };
        r.description = "Придает необычайную легкость шагу и ускоряет работу.";
        r.effectText = "+40% скорости бега на 30 секунд";
        r.formulaText = "1 Ягода + 2 Семени";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 7. Настойка ветра (с ягодным порошком)
    {
        PotionRecipe r;
        r.resultId = "speed_potion";
        r.name = "Настойка ветра";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"berry_powder", 1}, {"seeds", 2} };
        r.description = "Быстродействующий эликсир скорости длительного действия.";
        r.effectText = "+40% скорости бега на 30 секунд";
        r.formulaText = "1 Ягодный порошок + 2 Семени";
        r.discovered = false;
        recipes.push_back(r);
    }

    // === ЭЛИКСИРЫ РОСТА ===
    // 8. Эликсир роста (волокно и семя)
    {
        PotionRecipe r;
        r.resultId = "growth_potion";
        r.name = "Эликсир роста";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"fiber", 2}, {"seeds", 1} };
        r.description = "Магическая эссенция плодородия земли.";
        r.effectText = "Мгновенно ускоряет рост посевов вокруг";
        r.formulaText = "2 Волокна + 1 Семя";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 9. Природный стимулятор роста (листья и семя)
    {
        PotionRecipe r;
        r.resultId = "growth_potion";
        r.name = "Природный стимулятор роста";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"leaves", 2}, {"seeds", 1} };
        r.description = "Концентрат соков свежей листвы для проращивания культур.";
        r.effectText = "Мгновенно ускоряет рост посевов вокруг";
        r.formulaText = "2 Листьев + 1 Семя";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 10. Раствор плодородия (травяной порошок и семя)
    {
        PotionRecipe r;
        r.resultId = "growth_potion";
        r.name = "Раствор плодородия";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"herbal_powder", 1}, {"seeds", 1} };
        r.description = "Питательный концентрат пыльцы трав для всходов.";
        r.effectText = "Мгновенно ускоряет рост посевов вокруг";
        r.formulaText = "1 Травяной порошок + 1 Семя";
        r.discovered = false;
        recipes.push_back(r);
    }

    // 11. Великий эликсир роста
    {
        PotionRecipe r;
        r.resultId = "growth_potion";
        r.name = "Великий эликсир роста";
        r.baseId = "water_flask";
        r.requiredIngredients = { {"herbal_powder", 1}, {"fiber", 1}, {"seeds", 1} };
        r.description = "Усиленный раствор плодородия из травяной пыльцы.";
        r.effectText = "Мгновенно ускоряет рост посевов вокруг";
        r.formulaText = "1 Травяной порошок + 1 Волокно + 1 Семя";
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
