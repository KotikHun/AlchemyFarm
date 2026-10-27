#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "Inventory.hpp"
#include "ItemDatabase.hpp"
#include "DroppedItem.hpp"
#include "FloatingText.hpp"

struct Player;
class AlchemySystem;

// ============================================
// ВСПОМОГАТЕЛЬНАЯ ФУНКЦИЯ ДЛЯ UTF-8 (РУССКИЙ ТЕКСТ)
// ============================================
inline sf::String toUtf8(const std::string& str) {
    return sf::String::fromUtf8(str.begin(), str.end());
}

// ============================================
// ИНТЕРФЕЙС (UI)
// ============================================
struct UI {
    // Шрифт
    sf::Font font;

    // Текстуры иконок предметов
    std::map<std::string, sf::Texture> itemIcons;

    // Тексты статистики
    std::unique_ptr<sf::Text> dayText;
    std::unique_ptr<sf::Text> seedsText;
    std::unique_ptr<sf::Text> leavesText;

    // Элементы хотбара
    sf::RectangleShape hotbarBg;
    sf::RectangleShape slotShape;

    // Полоска дня
    sf::RectangleShape dayBarBg;
    sf::RectangleShape dayBar;

    // Окно инвентаря
    sf::RectangleShape inventoryBg;

    // Корзина для удаления
    sf::RectangleShape trashSlot;

    // Текст под курсором для тултипа
    std::string hoveredItemName;

    // Константы
    static constexpr int HOTBAR_SLOTS = 9;
    static constexpr float SLOT_SIZE = 50.f;
    static constexpr float SLOT_PADDING = 5.f;
    static constexpr float HOTBAR_MARGIN = 10.f;
    static constexpr float CHEST_OFFSET_Y = 170.f;

    UI();

    void loadIcon(const std::string& id, const std::string& path);
    static sf::Color getItemColor(const std::string& id);
    static std::string getItemLetter(const std::string& id);

    void drawItemIcon(sf::RenderWindow& window, float x, float y, const Item& item);
    void drawDroppedItem(sf::RenderWindow& window, const DroppedItem& drop);

    void drawHotbar(sf::RenderWindow& window, const Inventory& inv, int activeSlot, const ItemDatabase& itemDB);
    void drawHUD(sf::RenderWindow& window, int currentDay, const std::string& timeStr, bool isCurfew, int hour);
    void drawTexts(sf::RenderWindow& window, int currentDay, int seeds, int leaves);
    void drawDayBar(sf::RenderWindow& window, float dayProgress);

    void drawInventory(sf::RenderWindow& window,
        const Inventory& inv,
        int activeSlot,
        bool isOpen,
        bool isDragging,
        bool chestOpen,
        const ItemDatabase& itemDB,
        int currentTab = 0,
        int journalCategory = 0,
        const AlchemySystem* alchemy = nullptr);

    void drawCraftingTab(sf::RenderWindow& window, const Inventory& inv, const ItemDatabase& itemDB);
    void drawJournalTab(sf::RenderWindow& window, int selectedCategory, const AlchemySystem* alchemy);

    void drawInteractiveCauldron(sf::RenderWindow& window,
        const Inventory& inv,
        const AlchemySystem& alchemy,
        const ItemDatabase& itemDB);

    void drawMortarStation(sf::RenderWindow& window,
        const Inventory& inv,
        const AlchemySystem& alchemy,
        const ItemDatabase& itemDB);

    void drawChest(sf::RenderWindow& window,
        const Inventory& chestInv,
        int slotCount,
        bool isOpen,
        const ItemDatabase& itemDB);

    // Уведомление над хотбаром при переключении предмета в руке
    void drawActiveItemNotification(sf::RenderWindow& window, const std::string& itemName, float timer);

    // Всплывающие надписи (+2 Камень, +1 Ягода)
    void drawFloatingTexts(sf::RenderWindow& window, const std::vector<FloatingText>& texts);

    // Тултип с названием предмета под курсором
    void drawTooltip(sf::RenderWindow& window, sf::Vector2i mousePos, const std::string& text);

    // Подробная карточка предмета при зажатии ПКМ
    void drawDetailedItemCard(sf::RenderWindow& window, sf::Vector2i mousePos, const ItemData& data);

    // Всплывающее окно рецептов в котле
    void drawCauldronRecipeBook(sf::RenderWindow& window, float winX, float winY, const AlchemySystem& alchemy, const ItemDatabase& itemDB);

    // Отображение предмета в руках игрока
    void drawHeldItem(sf::RenderWindow& window, const Player& player, const Item& item);

    // Шкала энергии
    void drawEnergyBar(sf::RenderWindow& window, float currentEnergy, float maxEnergy);

    // Меню алхимии и крафта
    void drawAlchemy(sf::RenderWindow& window, const Inventory& inv, const ItemDatabase& itemDB);
};
