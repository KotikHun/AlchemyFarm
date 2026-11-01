#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "QuestManager.hpp"

// Forward declarations
struct UI;
struct Inventory;
struct ItemDatabase;

// ============================================
// ИНТЕРФЕЙС ДОСКИ ОБЪЯВЛЕНИЙ И ЛАВКИ
// ============================================

class OrdersBoardUI {
public:
    static sf::FloatRect getTabRect(int tabIndex);
    static sf::FloatRect getCloseButtonRect();
    static sf::FloatRect getQuestButtonRect(int questIndex);
    static sf::FloatRect getShopBuyButtonRect(int itemIndex);

    static void draw(sf::RenderWindow& window,
                     const sf::Font& font,
                     const UI& ui,
                     int activeTab,
                     const std::vector<QuestRecord>& quests,
                     const std::vector<ShopCatalogItem>& shopItems,
                     const Inventory& inv,
                     int playerGold,
                     const ItemDatabase& itemDB,
                     sf::Vector2f mousePos);
};
