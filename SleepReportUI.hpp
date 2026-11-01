#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Forward declarations
struct UI;

// ============================================
// ОТЧЁТ О ПРОДАЖАХ И СНЕ
// ============================================

struct SoldItemRecord {
    std::string id;
    std::string name;
    int count = 0;
    int priceEach = 0;
    int totalPrice = 0;
};

struct SleepReport {
    bool fullRest = true;
    int dayNumber = 1;
    std::vector<SoldItemRecord> soldItems;
    int totalEarned = 0;
    int previousGold = 0;
    int currentGold = 0;
};

class SleepReportUI {
public:
    static sf::FloatRect getNextButtonRect();

    static void draw(sf::RenderWindow& window,
                     const sf::Font& font,
                     const UI& ui,
                     const SleepReport& report,
                     float animTime,
                     bool isNextHovered);
};
