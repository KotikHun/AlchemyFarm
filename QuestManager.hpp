#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Forward declarations
struct Inventory;
struct Player;
class SoundManager;
struct FloatingText;

// ============================================
// ЗАКАЗЫ И ЛАВКА (КВЕСТЫ ЖИТЕЛЕЙ)
// ============================================

struct QuestRecord {
    std::string id;
    std::string clientName;
    std::string title;
    std::string description;
    std::string targetItemId;
    int targetCount = 1;
    int rewardGold = 60;
    std::string hint;
    bool isCompleted = false;
};

struct ShopCatalogItem {
    std::string itemId;
    std::string name;
    int price = 10;
    std::string desc;
};

class QuestManager {
public:
    std::vector<QuestRecord> dailyQuests;
    std::vector<ShopCatalogItem> shopCatalog;
    bool isOpen = false;
    int activeTab = 0; // 0 = Заказы, 1 = Лавка
    bool nearBoard = false;

    QuestManager();

    void initDailyQuests(int day);
    void initShopCatalog();
    void handleEvents(const sf::Event& event,
                      sf::Vector2f mpos,
                      Inventory& inv,
                      Player& player,
                      SoundManager& sound,
                      std::vector<FloatingText>& floatingTexts);
};
