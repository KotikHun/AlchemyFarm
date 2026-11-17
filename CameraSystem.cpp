#include "CameraSystem.hpp"
#include "House.hpp"
#include "Player.hpp"
#include <algorithm>
#include <cmath>

CameraSystem::CameraSystem()
    : currentCenter(480.f, 400.f), targetCenter(480.f, 400.f)
{
}

void CameraSystem::init(float virtualWidth, float virtualHeight)
{
    worldView.setSize({ virtualWidth, virtualHeight });
    worldView.setCenter({ virtualWidth / 2.f, virtualHeight / 2.f });

    uiView.setSize({ virtualWidth, virtualHeight });
    uiView.setCenter({ virtualWidth / 2.f, virtualHeight / 2.f });

    currentCenter = { virtualWidth / 2.f, virtualHeight / 2.f };
    targetCenter = currentCenter;
}

void CameraSystem::updateViewport(unsigned int windowWidth, unsigned int windowHeight, float virtualWidth, float virtualHeight)
{
    float windowRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    float viewRatio = virtualWidth / virtualHeight;

    float sizeX = 1.0f;
    float sizeY = 1.0f;
    float posX = 0.0f;
    float posY = 0.0f;

    if (windowRatio >= viewRatio)
    {
        // Окно шире -> черные полосы по бокам (pillarbox)
        sizeX = viewRatio / windowRatio;
        posX = (1.0f - sizeX) / 2.0f;
    }
    else
    {
        // Окно выше -> черные полосы сверху и снизу (letterbox)
        sizeY = windowRatio / viewRatio;
        posY = (1.0f - sizeY) / 2.0f;
    }

    sf::FloatRect vp({ posX, posY }, { sizeX, sizeY });
    uiView.setSize({ virtualWidth, virtualHeight });
    uiView.setCenter({ virtualWidth / 2.f, virtualHeight / 2.f });
    uiView.setViewport(vp);

    worldView.setViewport(vp);
    worldView.setSize({ virtualWidth * zoomLevel, virtualHeight * zoomLevel });
}

void CameraSystem::update(float dt, const sf::Vector2f& playerPos, bool isInside, float mapWidthTiles, float mapHeightTiles)
{
    // Плавный переход к целевому зуму
    if (std::abs(zoomLevel - targetZoom) > 0.001f)
    {
        float lerpZoom = 1.f - std::exp(-10.f * dt);
        zoomLevel += (targetZoom - zoomLevel) * lerpZoom;
        worldView.setSize({ 960.f * zoomLevel, 800.f * zoomLevel });
    }

    if (isInside)
    {
        // Внутри дома: центрируемся на комнате (House::ROOM_X = 240, ROOM_Y = 240, ROOM_W = 480, ROOM_H = 320)
        targetCenter = { House::ROOM_X + House::ROOM_W / 2.f, House::ROOM_Y + House::ROOM_H / 2.f };
    }
    else
    {
        // На улице: фокус на центре персонажа
        targetCenter = { playerPos.x + Player::SIZE / 2.f, playerPos.y + Player::SIZE / 2.f };
    }

    // Плавное скольжение камеры (lerp)
    float lerpFactor = 1.f - std::exp(-8.f * dt);
    currentCenter += (targetCenter - currentCenter) * lerpFactor;

    sf::Vector2f clampedCenter = currentCenter;

    if (!isInside)
    {
        float mapWidthPx = mapWidthTiles * 32.f;
        float mapHeightPx = mapHeightTiles * 32.f;

        float halfW = worldView.getSize().x / 2.f;
        float halfH = worldView.getSize().y / 2.f;

        // Если область видимости шире карты — центрируем по карте
        if (halfW * 2.f >= mapWidthPx)
        {
            clampedCenter.x = mapWidthPx / 2.f;
        }
        else
        {
            clampedCenter.x = std::clamp(currentCenter.x, halfW, mapWidthPx - halfW);
        }

        // Если область видимости выше карты — центрируем по карте
        if (halfH * 2.f >= mapHeightPx)
        {
            clampedCenter.y = mapHeightPx / 2.f;
        }
        else
        {
            clampedCenter.y = std::clamp(currentCenter.y, halfH, mapHeightPx - halfH);
        }
    }

    worldView.setCenter(clampedCenter);
}

void CameraSystem::handleZoom(float delta)
{
    // Зум колесиком мыши в диапазоне от 0.5x до 1.5x (1.0x — стандарт)
    targetZoom = std::clamp(targetZoom - delta * 0.1f, 0.5f, 1.5f);
}

sf::Vector2f CameraSystem::mapPixelToWorld(const sf::Vector2i& pixel, const sf::RenderWindow& window) const
{
    return window.mapPixelToCoords(pixel, worldView);
}

sf::Vector2f CameraSystem::mapPixelToUI(const sf::Vector2i& pixel, const sf::RenderWindow& window) const
{
    return window.mapPixelToCoords(pixel, uiView);
}
