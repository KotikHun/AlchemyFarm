#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Inventory.hpp"
#include "TimeSystem.hpp"
#include "World.hpp"
#include "House.hpp"
#include "Chest.hpp"
#include "Furniture.hpp"
#include "AlchemySystem.hpp"

// ============================================
// ИНФОРМАЦИЯ О СЛОТЕ СОХРАНЕНИЯ (МЕТАДАННЫЕ)
// ============================================
struct SaveSlotInfo {
    int slotIndex = 1;              // 1, 2, 3
    bool exists = false;
    std::string farmName = "Ферма Алхимика";
    int day = 1;
    int gold = 100;
    std::string timeStr = "06:00";
    std::string timestamp = "";     // Дата и время сохранения (напр. "29.10 10:45")
};

// ============================================
// МЕНЕДЖЕР СОХРАНЕНИЙ И ЗАГРУЗКИ
// ============================================
class SaveManager {
public:
    static constexpr int MAX_SLOTS = 3;

    // Путь к файлу слота (data/saves/slot_X.json)
    static std::string getSaveFilePath(int slot);

    // Проверка наличия сохранения
    static bool doesSlotExist(int slot);

    // Получение метаданных одного слота
    static SaveSlotInfo getSlotInfo(int slot);

    // Получение информации о всех 3 слотах
    static std::vector<SaveSlotInfo> getAllSlotsInfo();

    // Удаление сохранения
    static bool deleteSlot(int slot);

    // Сохранение полного состояния игры
    static bool saveGame(int slot,
                         const Player& player,
                         const Inventory& inv,
                         const TimeSystem& timeSystem,
                         const World& world,
                         const House& house,
                         const std::vector<Chest>& chests,
                         const std::vector<Furniture>& furniture,
                         const AlchemySystem& alchemy);

    // Загрузка полного состояния игры
    static bool loadGame(int slot,
                         Player& player,
                         Inventory& inv,
                         TimeSystem& timeSystem,
                         World& world,
                         House& house,
                         std::vector<Chest>& chests,
                         std::vector<Furniture>& furniture,
                         AlchemySystem& alchemy);

private:
    static std::string getCurrentTimestampString();
};
