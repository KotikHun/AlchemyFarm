#include "SaveManager.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

std::string SaveManager::getSaveFilePath(int slot)
{
    return "data/saves/slot_" + std::to_string(slot) + ".json";
}

bool SaveManager::doesSlotExist(int slot)
{
    if (slot < 1 || slot > MAX_SLOTS) return false;
    return fs::exists(getSaveFilePath(slot));
}

std::string SaveManager::getCurrentTimestampString()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm, "%d.%m.%Y %H:%M");
    return ss.str();
}

SaveSlotInfo SaveManager::getSlotInfo(int slot)
{
    SaveSlotInfo info;
    info.slotIndex = slot;
    info.exists = false;

    if (slot < 1 || slot > MAX_SLOTS) return info;

    std::string path = getSaveFilePath(slot);
    if (!fs::exists(path)) return info;

    try
    {
        std::ifstream file(path);
        if (!file.is_open()) return info;

        json j;
        file >> j;

        info.exists = true;
        info.farmName = j.value("farmName", "Ферма Алхимика");
        info.timestamp = j.value("timestamp", "");

        if (j.contains("time"))
        {
            info.day = j["time"].value("currentDay", 1);
            float mins = j["time"].value("timeInMinutes", 0.f);
            int totalM = static_cast<int>(mins) + 6 * 60;
            int h = (totalM / 60) % 24;
            int m = (totalM % 60) / 10 * 10;
            std::ostringstream tss;
            tss << (h < 10 ? "0" : "") << h << ":" << (m < 10 ? "0" : "") << m;
            info.timeStr = tss.str();
        }
        if (j.contains("player"))
        {
            info.gold = j["player"].value("gold", 100);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "SaveManager: error reading slot info: " << e.what() << std::endl;
        info.exists = false;
    }

    return info;
}

std::vector<SaveSlotInfo> SaveManager::getAllSlotsInfo()
{
    std::vector<SaveSlotInfo> res;
    res.reserve(MAX_SLOTS);
    for (int i = 1; i <= MAX_SLOTS; i++)
    {
        res.push_back(getSlotInfo(i));
    }
    return res;
}

bool SaveManager::deleteSlot(int slot)
{
    if (slot < 1 || slot > MAX_SLOTS) return false;
    std::string path = getSaveFilePath(slot);
    if (fs::exists(path))
    {
        std::error_code ec;
        fs::remove(path, ec);
        return !ec;
    }
    return true;
}

bool SaveManager::saveGame(int slot,
                           const Player& player,
                           const Inventory& inv,
                           const TimeSystem& timeSystem,
                           const World& world,
                           const House& house,
                           const std::vector<Chest>& chests,
                           const std::vector<Furniture>& furniture,
                           const AlchemySystem& alchemy)
{
    if (slot < 1 || slot > MAX_SLOTS) return false;

    try
    {
        fs::create_directories("data/saves");

        json j;
        j["version"] = 1;
        j["slot"] = slot;
        j["farmName"] = "Ферма Алхимика";
        j["timestamp"] = getCurrentTimestampString();

        // 1. Игрок
        sf::Vector2f ppos = player.shape.getPosition();
        j["player"] = {
            { "posX", ppos.x },
            { "posY", ppos.y },
            { "energy", player.energy },
            { "maxEnergy", player.maxEnergy },
            { "isExhausted", player.isExhausted },
            { "gold", player.gold },
            { "dirX", player.dirX },
            { "dirY", player.dirY }
        };

        // 2. Время
        j["time"] = {
            { "currentDay", timeSystem.currentDay },
            { "timeInMinutes", timeSystem.timeInMinutes },
            { "isRaining", timeSystem.isRaining }
        };

        // 3. Дом
        j["house"] = {
            { "isInside", house.isInside }
        };

        // 4. Инвентарь игрока (36 слотов)
        json invArr = json::array();
        for (int i = 0; i < Inventory::SIZE; i++)
        {
            const Item& itm = inv.getSlot(i);
            invArr.push_back({
                { "id", itm.id },
                { "count", itm.count }
            });
        }
        j["inventory"] = invArr;

        // 5. Мир: тайлы
        json tilesArr = json::array();
        for (int y = 0; y < World::HEIGHT; y++)
        {
            for (int x = 0; x < World::WIDTH; x++)
            {
                const Tile& t = world.map[y][x];
                tilesArr.push_back({
                    { "x", x },
                    { "y", y },
                    { "t", t.type },
                    { "s", t.plantStage },
                    { "c", t.cropType },
                    { "w", t.watered },
                    { "d", t.daysGrown },
                    { "v", t.variant }
                });
            }
        }
        j["world"]["tiles"] = tilesArr;

        // Мир: объекты (сорняки, камни, пни)
        json objArr = json::array();
        for (const auto& obj : world.objects)
        {
            objArr.push_back({
                { "x", obj.tileX },
                { "y", obj.tileY },
                { "type", static_cast<int>(obj.type) },
                { "hp", obj.hp },
                { "maxHp", obj.maxHp },
                { "v", obj.variant }
            });
        }
        j["world"]["objects"] = objArr;

        // Мир: деревья
        json treeArr = json::array();
        for (const auto& tr : world.trees)
        {
            treeArr.push_back({
                { "x", tr.position.x },
                { "y", tr.position.y },
                { "hp", tr.hp },
                { "stage", tr.stage },
                { "daysGrown", tr.daysGrown }
            });
        }
        j["world"]["trees"] = treeArr;

        // 6. Сундуки
        json chestsArr = json::array();
        for (const auto& c : chests)
        {
            json cJson;
            cJson["type"] = c.type;
            cJson["posX"] = c.position.x;
            cJson["posY"] = c.position.y;
            cJson["isInside"] = c.isInside;
            cJson["slotCount"] = c.slotCount;

            json cInv = json::array();
            for (int i = 0; i < c.slotCount; i++)
            {
                const Item& ci = c.inventory.getSlot(i);
                cInv.push_back({
                    { "id", ci.id },
                    { "count", ci.count }
                });
            }
            cJson["items"] = cInv;
            chestsArr.push_back(cJson);
        }
        j["chests"] = chestsArr;

        // 7. Мебель
        json furnArr = json::array();
        for (const auto& f : furniture)
        {
            furnArr.push_back({
                { "id", f.id },
                { "posX", f.position.x },
                { "posY", f.position.y },
                { "isInside", f.isInside }
            });
        }
        j["furniture"] = furnArr;

        // 8. Алхимия
        json alcJson;
        json discRecipes = json::array();
        for (const auto& r : alchemy.recipes)
        {
            if (r.discovered)
            {
                discRecipes.push_back(r.resultId);
            }
        }
        alcJson["unlocked"] = discRecipes;
        alcJson["cauldronBase"] = {
            { "id", alchemy.cauldronBase.id },
            { "count", alchemy.cauldronBase.count }
        };
        json cIngArr = json::array();
        for (int i = 0; i < 6; i++)
        {
            cIngArr.push_back({
                { "id", alchemy.cauldronIngredients[i].id },
                { "count", alchemy.cauldronIngredients[i].count }
            });
        }
        alcJson["cauldronIngredients"] = cIngArr;
        alcJson["mortarInput"] = {
            { "id", alchemy.mortarInput.id },
            { "count", alchemy.mortarInput.count }
        };
        alcJson["mortarOutput"] = {
            { "id", alchemy.mortarOutput.id },
            { "count", alchemy.mortarOutput.count }
        };
        j["alchemy"] = alcJson;

        std::ofstream out(getSaveFilePath(slot));
        if (!out.is_open()) return false;
        out << j.dump(4);
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "SaveManager: save failed: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::loadGame(int slot,
                           Player& player,
                           Inventory& inv,
                           TimeSystem& timeSystem,
                           World& world,
                           House& house,
                           std::vector<Chest>& chests,
                           std::vector<Furniture>& furniture,
                           AlchemySystem& alchemy)
{
    if (slot < 1 || slot > MAX_SLOTS) return false;

    std::string path = getSaveFilePath(slot);
    if (!fs::exists(path)) return false;

    try
    {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        json j;
        file >> j;

        // 1. Игрок
        if (j.contains("player"))
        {
            const auto& pj = j["player"];
            float px = pj.value("posX", 140.f);
            float py = pj.value("posY", 140.f);
            player.shape.setPosition({ px, py });
            if (player.sprite)
            {
                player.sprite->setPosition({ px + Player::SIZE / 2.f, py + Player::SIZE / 2.f });
            }
            player.energy = pj.value("energy", 100.f);
            player.maxEnergy = pj.value("maxEnergy", 100.f);
            player.isExhausted = pj.value("isExhausted", false);
            player.gold = pj.value("gold", 100);
            player.dirX = pj.value("dirX", 0);
            player.dirY = pj.value("dirY", 1);
            player.isActing = false;
            player.actionTimer = 0.f;
        }

        // 2. Время
        if (j.contains("time"))
        {
            const auto& tj = j["time"];
            timeSystem.currentDay = tj.value("currentDay", 1);
            timeSystem.timeInMinutes = tj.value("timeInMinutes", 0.f);
            timeSystem.isRaining = tj.value("isRaining", false);
        }

        // 3. Дом
        if (j.contains("house"))
        {
            house.isInside = j["house"].value("isInside", false);
        }

        // 4. Инвентарь
        if (j.contains("inventory"))
        {
            inv = Inventory();
            const auto& invArr = j["inventory"];
            for (size_t i = 0; i < invArr.size() && i < Inventory::SIZE; i++)
            {
                std::string id = invArr[i].value("id", "");
                int count = invArr[i].value("count", 0);
                if (!id.empty() && count > 0)
                {
                    inv.setSlot(static_cast<int>(i), Item{ id, count });
                }
            }
        }

        // 5. Мир: тайлы
        if (j.contains("world") && j["world"].contains("tiles"))
        {
            for (const auto& tj : j["world"]["tiles"])
            {
                int x = tj.value("x", 0);
                int y = tj.value("y", 0);
                if (x >= 0 && x < World::WIDTH && y >= 0 && y < World::HEIGHT)
                {
                    Tile& t = world.map[y][x];
                    t.type = tj.value("t", 0);
                    t.plantStage = tj.value("s", 0);
                    t.cropType = tj.value("c", 0);
                    t.watered = tj.value("w", false);
                    t.daysGrown = tj.value("d", 0);
                    t.variant = tj.value("v", 0);
                }
            }
        }

        // Мир: объекты
        if (j.contains("world") && j["world"].contains("objects"))
        {
            world.objects.clear();
            for (const auto& oj : j["world"]["objects"])
            {
                WorldObject obj;
                obj.tileX = oj.value("x", 0);
                obj.tileY = oj.value("y", 0);
                obj.type = static_cast<ObjectType>(oj.value("type", 0));
                obj.hp = oj.value("hp", 1);
                obj.maxHp = oj.value("maxHp", 1);
                obj.variant = oj.value("v", 0);
                world.objects.push_back(obj);
            }
        }

        // Мир: деревья
        if (j.contains("world") && j["world"].contains("trees"))
        {
            world.trees.clear();
            for (const auto& trj : j["world"]["trees"])
            {
                Tree tr;
                tr.position = { trj.value("x", 0.f), trj.value("y", 0.f) };
                tr.hp = trj.value("hp", 4);
                tr.stage = trj.value("stage", 2);
                tr.daysGrown = trj.value("daysGrown", (tr.stage == 2) ? 7 : 0);
                world.trees.push_back(tr);
            }
        }

        // 6. Сундуки
        if (j.contains("chests"))
        {
            chests.clear();
            for (const auto& cj : j["chests"])
            {
                Chest c;
                c.type = cj.value("type", "wooden");
                c.position = { cj.value("posX", 0.f), cj.value("posY", 0.f) };
                c.isInside = cj.value("isInside", false);
                c.slotCount = cj.value("slotCount", 18);
                c.isOpen = false;
                c.inventory = Inventory();

                if (cj.contains("items"))
                {
                    const auto& cInv = cj["items"];
                    for (size_t i = 0; i < cInv.size() && i < static_cast<size_t>(c.slotCount); i++)
                    {
                        std::string id = cInv[i].value("id", "");
                        int count = cInv[i].value("count", 0);
                        if (!id.empty() && count > 0)
                        {
                            c.inventory.setSlot(static_cast<int>(i), Item{ id, count });
                        }
                    }
                }
                chests.push_back(c);
            }
        }

        // 7. Мебель
        if (j.contains("furniture"))
        {
            furniture.clear();
            for (const auto& fj : j["furniture"])
            {
                Furniture f;
                f.id = fj.value("id", "");
                f.position = { fj.value("posX", 0.f), fj.value("posY", 0.f) };
                f.isInside = fj.value("isInside", false);
                furniture.push_back(f);
            }
        }

        // 8. Алхимия
        if (j.contains("alchemy"))
        {
            const auto& aj = j["alchemy"];
            if (aj.contains("unlocked"))
            {
                std::vector<std::string> unlocked = aj["unlocked"].get<std::vector<std::string>>();
                for (auto& r : alchemy.recipes)
                {
                    if (r.resultId == "chamomile_decoction" && r.name == "Ромашковый отвар")
                    {
                        r.discovered = true;
                    }
                    else if (r.resultId == "berry_juice" && r.name == "Ягодный морс")
                    {
                        r.discovered = true;
                    }
                    else
                    {
                        r.discovered = (std::find(unlocked.begin(), unlocked.end(), r.resultId) != unlocked.end() ||
                                        std::find(unlocked.begin(), unlocked.end(), r.name) != unlocked.end());
                    }
                }
            }
            if (aj.contains("cauldronBase"))
            {
                alchemy.cauldronBase = Item{
                    aj["cauldronBase"].value("id", ""),
                    aj["cauldronBase"].value("count", 0)
                };
            }
            if (aj.contains("cauldronIngredients"))
            {
                const auto& ciArr = aj["cauldronIngredients"];
                for (size_t i = 0; i < 6 && i < ciArr.size(); i++)
                {
                    alchemy.cauldronIngredients[i] = Item{
                        ciArr[i].value("id", ""),
                        ciArr[i].value("count", 0)
                    };
                }
            }
            if (aj.contains("mortarInput"))
            {
                alchemy.mortarInput = Item{
                    aj["mortarInput"].value("id", ""),
                    aj["mortarInput"].value("count", 0)
                };
            }
            if (aj.contains("mortarOutput"))
            {
                alchemy.mortarOutput = Item{
                    aj["mortarOutput"].value("id", ""),
                    aj["mortarOutput"].value("count", 0)
                };
            }
            alchemy.isCauldronOpen = false;
            alchemy.isTableOpen = false;
            alchemy.showRecipes = false;
            alchemy.isBrewing = false;
            alchemy.isGrinding = false;
        }

        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "SaveManager: load failed: " << e.what() << std::endl;
        return false;
    }
}
