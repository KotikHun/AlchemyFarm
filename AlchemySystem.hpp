#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>
#include <array>
#include "Item.hpp"
#include "Inventory.hpp"

// ============================================
// РЕЦЕПТ ЗЕЛЬЯ
// ============================================
struct PotionRecipe {
    std::string resultId;
    std::string name;
    std::string baseId;                              // "water_flask"
    std::map<std::string, int> requiredIngredients; // Точное количество ингредиентов
    std::string description;
    std::string effectText;
    std::string formulaText;                         // Текстовое описание формулы
    bool discovered = false;                         // Открыт ли рецепт в дневнике
};

// ============================================
// РЕЗУЛЬТАТ ВАРКИ
// ============================================
struct BrewResult {
    bool success = false;
    std::string resultId;
    std::string resultName;
    std::string message;
};

// ============================================
// СИСТЕМА АЛХИМИИ
// ============================================
class AlchemySystem {
public:
    std::vector<PotionRecipe> recipes;

    // Состояние интерактивного котла
    bool isCauldronOpen = false;
    bool showRecipes = false;
    std::array<Item, 6> cauldronIngredients;
    Item cauldronBase;
    bool isBrewing = false;
    float brewTimer = 0.f;
    static constexpr float BREW_DURATION = 1.2f;

    // Состояние алхимического стола (ступка и пестик)
    bool isTableOpen = false;
    Item mortarInput;
    Item mortarOutput;
    bool isGrinding = false;
    float grindTimer = 0.f;
    static constexpr float GRIND_DURATION = 0.8f;

    AlchemySystem();

    // Проверка текущего рецепта в котле
    const PotionRecipe* findMatchingRecipe() const;

    // Запуск варки
    bool tryStartBrewing(std::string& outError);

    // Завершение варки
    BrewResult finishBrewing();

    // Ступка и пестик на алхимическом столе
    bool canGrind() const;
    std::string getGrindResultId() const;
    void startGrinding();
    bool finishGrinding();

    // Справочник для дневника
    const std::vector<PotionRecipe>& getRecipes() const { return recipes; }
    void unlockRecipe(const std::string& resultId);
};
