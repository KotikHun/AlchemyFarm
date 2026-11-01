#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// Forward declarations
struct UI;
struct Inventory;
class AlchemySystem;
struct ItemDatabase;

// ============================================
// ИНТЕРФЕЙС АЛХИМИИ (КОТЕЛ, СТУПКА, РЕЦЕПТЫ)
// ============================================

class AlchemyUI {
public:
    static void drawInteractiveCauldron(sf::RenderWindow& window,
                                        const sf::Font& font,
                                        UI& ui,
                                        const Inventory& inv,
                                        const AlchemySystem& alchemy,
                                        const ItemDatabase& itemDB);

    static void drawCauldronRecipeBook(sf::RenderWindow& window,
                                       const sf::Font& font,
                                       UI& ui,
                                       float winX,
                                       float winY,
                                       const AlchemySystem& alchemy,
                                       const ItemDatabase& itemDB);

    static void drawMortarStation(sf::RenderWindow& window,
                                  const sf::Font& font,
                                  UI& ui,
                                  const Inventory& inv,
                                  const AlchemySystem& alchemy,
                                  const ItemDatabase& itemDB);
};
