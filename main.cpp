#include <SFML/Graphics.hpp>
#include <optional>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Alchemy Farm");

    const float TILE_SIZE = 32.f;

    sf::RectangleShape tile({ TILE_SIZE, TILE_SIZE });
    tile.setFillColor(sf::Color(139, 90, 43));
    tile.setPosition({ 384.f, 284.f });

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(tile);
        window.display();
    }

    return 0;
}