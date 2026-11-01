#include "TimeSystem.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

TimeSystem::TimeSystem()
{
    timeInMinutes = 0.f;
    currentDay = 1;
}

void TimeSystem::update(float dt)
{
    if (!isFadingOut && !isFadingIn)
    {
        timeInMinutes += dt * MINUTES_PER_SECOND;
    }

    if (isFadingOut)
    {
        fadeAlpha += fadeSpeed * dt;
        if (fadeAlpha >= 255.f)
        {
            fadeAlpha = 255.f;
            isFadingOut = false;
        }
    }
    else if (isFadingIn)
    {
        fadeAlpha -= fadeSpeed * dt;
        if (fadeAlpha <= 0.f)
        {
            fadeAlpha = 0.f;
            isFadingIn = false;
        }
    }
}

void TimeSystem::advanceDay(bool /*fullRest*/)
{
    currentDay++;
    timeInMinutes = 0.f; // Просыпаемся ровно в 06:00
    isFadingOut = false;
    isFadingIn = true;
    fadeAlpha = 255.f;
    isRaining = (rand() % 100 < 30); // 30% шанс дождя
}

int TimeSystem::getHour() const
{
    int totalHours = 6 + static_cast<int>(timeInMinutes / 60.f);
    if (totalHours >= 24) totalHours -= 24;
    return totalHours;
}

int TimeSystem::getMinute() const
{
    int mins = static_cast<int>(timeInMinutes) % 60;
    // Округляем до ближайших 10 минут, как в Stardew
    return (mins / 10) * 10;
}

std::string TimeSystem::getTimeString() const
{
    int h = getHour();
    int m = getMinute();
    std::ostringstream ss;
    ss << std::setw(2) << std::setfill('0') << h << ":"
       << std::setw(2) << std::setfill('0') << m;
    return ss.str();
}

bool TimeSystem::isCurfew() const
{
    // От 18 часов с момента пробуждения (т.е. с 00:00 до 02:00)
    return timeInMinutes >= (18.f * 60.f);
}

bool TimeSystem::hasPassedOut() const
{
    return timeInMinutes >= MAX_DAY_MINUTES;
}

void TimeSystem::triggerFadeOut()
{
    isFadingOut = true;
    isFadingIn = false;
}

void TimeSystem::triggerFadeIn()
{
    isFadingIn = true;
    isFadingOut = false;
}

void TimeSystem::drawNightOverlay(sf::RenderWindow& window, const std::vector<std::pair<sf::Vector2f, float>>& lightSources)
{
    // Расчет темноты в зависимости от времени суток:
    // 06:00 - 18:00 (0 - 720 мин): День (alpha = 0)
    // 18:00 - 21:00 (720 - 900 мин): Закат (alpha 0 -> 90)
    // 21:00 - 00:00 (900 - 1080 мин): Ночь (alpha 90 -> 180)
    // 00:00 - 02:00 (1080 - 1200 мин): Полночь / Комендантский час (alpha 180 -> 210)
    float alpha = 0.f;
    sf::Color tint(12, 16, 45); // Глубокий ночной индиго

    if (timeInMinutes < 720.f)
    {
        alpha = 0.f;
    }
    else if (timeInMinutes < 900.f)
    {
        float t = (timeInMinutes - 720.f) / 180.f;
        alpha = t * 95.f;
        // Теплый оттенок заката
        tint = sf::Color(
            static_cast<std::uint8_t>(20 + 30 * (1.f - t)),
            static_cast<std::uint8_t>(18 + 10 * (1.f - t)),
            static_cast<std::uint8_t>(45)
        );
    }
    else if (timeInMinutes < 1080.f)
    {
        float t = (timeInMinutes - 900.f) / 180.f;
        alpha = 95.f + t * 85.f; // до 180
        tint = sf::Color(10, 15, 45);
    }
    else
    {
        float t = std::min(1.f, (timeInMinutes - 1080.f) / 120.f);
        alpha = 180.f + t * 35.f; // до 215
        tint = sf::Color(8, 12, 40);
    }

    if (alpha <= 2.f) return;

    sf::Vector2u winSize = { 960, 800 };

    // Создаем или обновляем текстуру света при необходимости
    if (!isLightingInitialized || lightTexture.getSize() != winSize)
    {
        if (lightTexture.resize(winSize))
        {
            isLightingInitialized = true;
        }
        else
        {
            // Если ресайз не удался, рисуем простой полупрозрачный прямоугольник
            sf::RectangleShape darkRect(sf::Vector2f(960.f, 800.f));
            darkRect.setFillColor(sf::Color(tint.r, tint.g, tint.b, static_cast<std::uint8_t>(alpha)));
            window.draw(darkRect);
            return;
        }
    }

    // Заливаем текстуру темнотой
    lightTexture.clear(sf::Color(tint.r, tint.g, tint.b, static_cast<std::uint8_t>(alpha)));

    // «Вырезаем» мягкие круги света для каждого источника света
    for (const auto& [pos, radius] : lightSources)
    {
        // Рисуем концентрические круги для мягкого радиального градиента
        int steps = 10;
        for (int i = steps; i >= 1; i--)
        {
            float r = radius * (static_cast<float>(i) / steps);
            sf::CircleShape glow(r);
            glow.setOrigin({ r, r });
            glow.setPosition(pos);

            // Плавное рассеивание
            float factor = 1.f - (static_cast<float>(i) / steps);
            int lightAlpha = static_cast<int>(alpha * factor * 0.95f);
            glow.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(lightAlpha)));

            // Используем sf::BlendSubtract или BlendNone для мягкого вычитания
            lightTexture.draw(glow, sf::BlendNone);
        }
    }

    lightTexture.display();

    sf::Sprite overlaySprite(lightTexture.getTexture());
    window.draw(overlaySprite);
}

void TimeSystem::drawFade(sf::RenderWindow& window)
{
    if (fadeAlpha <= 1.f) return;

    sf::RectangleShape fadeRect(sf::Vector2f(960.f, 800.f));
    fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
    window.draw(fadeRect);
}
