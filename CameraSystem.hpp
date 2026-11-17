#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

// ============================================
// СИСТЕМА КАМЕРЫ (СЛЕЖЕНИЕ, ДВА VIEW, ЗУМ)
// ============================================

class CameraSystem {
public:
    sf::View worldView; // Следит за миром, зуммируется, двигается за игроком
    sf::View uiView;    // Фиксированный слой UI (960x800) с сохранением пропорций (Letterbox)

    float zoomLevel = 1.0f;
    float targetZoom = 1.0f;

    sf::Vector2f currentCenter;
    sf::Vector2f targetCenter;

    CameraSystem();

    void init(float virtualWidth, float virtualHeight);
    void updateViewport(unsigned int windowWidth, unsigned int windowHeight, float virtualWidth, float virtualHeight);
    void update(float dt, const sf::Vector2f& playerPos, bool isInside, float mapWidthTiles, float mapHeightTiles);
    void handleZoom(float delta);

    sf::Vector2f mapPixelToWorld(const sf::Vector2i& pixel, const sf::RenderWindow& window) const;
    sf::Vector2f mapPixelToUI(const sf::Vector2i& pixel, const sf::RenderWindow& window) const;
};
