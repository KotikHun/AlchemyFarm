#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// ============================================
// СИСТЕМА ВРЕМЕНИ И ОСВЕЩЕНИЯ (Stardew-style)
// ============================================
// Отвечает за ход часов, смену дня/ночи, ночное освещение
// и комендантский час (00:00 - 02:00).

class TimeSystem {
public:
    int currentDay = 1;
    float timeInMinutes = 0.f;  // 0 мин = 06:00, 1080 мин = 00:00, 1200 мин = 02:00
    bool isRaining = false;     // Дождливая погода

    // Темп времени: 10 игровых минут каждые 3.5 сек (~21 сек на 1 час, ~7 минут на день)
    static constexpr float MINUTES_PER_SECOND = 10.f / 3.5f;
    static constexpr float MAX_DAY_MINUTES = 20.f * 60.f; // 1200 минут (до 02:00)

    // Затемнение и освещение
    sf::RenderTexture lightTexture;
    bool isLightingInitialized = false;

    // Плавное затемнение при засыпании / потере сознания
    float fadeAlpha = 0.f;
    bool isFadingOut = false;
    bool isFadingIn = false;
    float fadeSpeed = 350.f;

    TimeSystem();

    void update(float dt);
    void advanceDay(bool fullRest);

    int getHour() const;
    int getMinute() const;
    std::string getTimeString() const;

    bool isCurfew() const;      // 00:00 - 02:00
    bool hasPassedOut() const;   // 02:00 наступило

    void triggerFadeOut();
    void triggerFadeIn();

    void drawNightOverlay(sf::RenderWindow& window, const std::vector<std::pair<sf::Vector2f, float>>& lightSources);
    void drawFade(sf::RenderWindow& window);
};
