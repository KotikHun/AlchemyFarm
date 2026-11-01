#pragma once
#include "TimeSystem.hpp"
#include "Player.hpp"
#include "Inventory.hpp"
#include "World.hpp"
#include "House.hpp"
#include "Chest.hpp"
#include "Furniture.hpp"
#include "AlchemySystem.hpp"
#include "SoundManager.hpp"
#include "FloatingText.hpp"
#include "ItemDatabase.hpp"
#include "SleepReportUI.hpp"
#include "QuestManager.hpp"
#include "SaveManager.hpp"
#include <vector>

class DayNightCycle
{
public:
    static void sleepInBed(bool fullRest,
                           bool& isTransitioningSleep,
                           bool isTransitioningWake,
                           float& fadeTarget,
                           SleepReport& sleepReport,
                           SoundManager& sound);

    static void triggerSleep(bool fullRest,
                             SleepReport& sleepReport,
                             TimeSystem& timeSystem,
                             Player& player,
                             std::vector<Chest>& chests,
                             const ItemDatabase& itemDB,
                             bool& isInventoryOpen,
                             AlchemySystem& alchemy,
                             Chest* nearbyChest,
                             bool& isSleepNextHovered);

    static void wakeUpFromSleep(SleepReport& sleepReport,
                                TimeSystem& timeSystem,
                                World& world,
                                House& house,
                                Player& player,
                                QuestManager& quests,
                                Inventory& inv,
                                std::vector<Chest>& chests,
                                std::vector<Furniture>& furniture,
                                AlchemySystem& alchemy,
                                std::vector<FloatingText>& floatingTexts,
                                int currentSaveSlot,
                                std::vector<SaveSlotInfo>& cachedSaveSlots);
};
