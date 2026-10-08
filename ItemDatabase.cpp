#include "ItemDatabase.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>

// ============================================
// ЗАГРУЗКА ИЗ ФАЙЛА
// ============================================
bool ItemDatabase::loadFromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        sf::err() << "ItemDatabase: cannot open " << path << std::endl;
        return false;
    }

    nlohmann::json j;
    try
    {
        file >> j;
    }
    catch (const std::exception& e)
    {
        sf::err() << "ItemDatabase: JSON parse error: " << e.what() << std::endl;
        return false;
    }

    // Очищаем
    items.clear();
    idToIndex.clear();

    // Загружаем предметы
    if (!j.contains("items") || !j["items"].is_array())
    {
        sf::err() << "ItemDatabase: no 'items' array in JSON" << std::endl;
        return false;
    }

    for (const auto& itemJson : j["items"])
    {
        ItemData data;
        data.id = itemJson.value("id", "");
        data.name = itemJson.value("name", "Unknown");
        data.maxStack = itemJson.value("maxStack", 1);
        data.isTool = itemJson.value("isTool", false);

        if (data.id.empty())
        {
            sf::err() << "ItemDatabase: item with empty id, skipped" << std::endl;
            continue;
        }

        int index = static_cast<int>(items.size());
        idToIndex[data.id] = index;
        items.push_back(data);
    }

    std::cout << "ItemDatabase: loaded " << items.size() << " items" << std::endl;
    return true;
}

// ============================================
// ПОИСК ПО ID
// ============================================
const ItemData* ItemDatabase::find(const std::string& id) const
{
    auto it = idToIndex.find(id);
    if (it == idToIndex.end())
        return nullptr;
    return &items[it->second];
}