#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct SaveSlotInfo;

// ============================================
// СОСТОЯНИЯ МЕНЮ И НАСТРОЙКИ
// ============================================

enum class MenuModal {
    None,
    Saves,
    Settings,
    About
};

struct GameSettings {
    int soundVolume = 80;
    int musicVolume = 70;
    bool fullscreen = false;
};

struct MenuParticle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float alpha = 200.f;
    float size = 3.f;
    float life = 1.f;
    float maxLife = 4.f;
    sf::Color color;
};

// ============================================
// ИНТЕРФЕЙС ГЛАВНОГО МЕНЮ, ЗАГРУЗКИ И ПАУЗЫ
// ============================================

class MenuUI {
public:
    static sf::FloatRect getMainMenuButtonRect(int index);
    static sf::FloatRect getPauseMenuButtonRect(int index);

    static sf::FloatRect getAboutBackButtonRect();

    static sf::FloatRect getSettingsBackButtonRect();
    static sf::FloatRect getSettingsSoundMinusRect();
    static sf::FloatRect getSettingsSoundPlusRect();
    static sf::FloatRect getSettingsMusicMinusRect();
    static sf::FloatRect getSettingsMusicPlusRect();

    static sf::FloatRect getSavesBackButtonRect();
    static sf::FloatRect getSavesActionBtnRect(int slotIndex);
    static sf::FloatRect getSavesDeleteBtnRect(int slotIndex);
    static sf::FloatRect getSavesEmptyBtnRect(int slotIndex);

    static void drawMainMenu(sf::RenderWindow& window,
                             const sf::Font& font,
                             float animTime,
                             int hoveredButton,
                             MenuModal modal,
                             const GameSettings& settings,
                             const std::vector<SaveSlotInfo>& saveSlots,
                             bool isSaveMode,
                             int pendingDeleteSlot,
                             const std::vector<MenuParticle>& particles);

    static void drawLoadingScreen(sf::RenderWindow& window,
                                  const sf::Font& font,
                                  float progress,
                                  const std::string& stageText,
                                  const std::string& tipText,
                                  float animTime);

    static void drawPauseMenu(sf::RenderWindow& window,
                              const sf::Font& font,
                              int hoveredButton,
                              MenuModal modal,
                              const GameSettings& settings,
                              const std::vector<SaveSlotInfo>& saveSlots,
                              bool isSaveMode,
                              int pendingDeleteSlot);

    static void drawSettingsModal(sf::RenderWindow& window,
                                  const sf::Font& font,
                                  const GameSettings& settings,
                                  int hoveredButton);

    static void drawSavesModal(sf::RenderWindow& window,
                               const sf::Font& font,
                               const std::vector<SaveSlotInfo>& saveSlots,
                               bool isSaveMode,
                               int pendingDeleteSlot);

    static void drawAboutModal(sf::RenderWindow& window,
                               const sf::Font& font,
                               int hoveredButton);
};
