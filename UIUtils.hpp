#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <cmath>

inline sf::String toUtf8(const std::string& str) {
    return sf::String::fromUtf8(str.begin(), str.end());
}

inline bool isInside(float mx, float my, float x, float y, float size)
{
    return mx >= x && mx < x + size && my >= y && my < y + size;
}

inline bool isInsideRect(float mx, float my, float x, float y, float w, float h)
{
    return mx >= x && mx < x + w && my >= y && my < y + h;
}

inline sf::Vector2i getMousePos(const sf::RenderWindow& window)
{
    sf::Vector2f c = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    return sf::Vector2i(static_cast<int>(std::round(c.x)), static_cast<int>(std::round(c.y)));
}

inline std::string wrapString(const sf::Font& font, const std::string& input, unsigned int charSize, float maxW)
{
    sf::Text tester(font);
    tester.setCharacterSize(charSize);
    std::istringstream stream(input);
    std::string word;
    std::string result;
    std::string currentLine;

    while (stream >> word)
    {
        std::string testLine = currentLine.empty() ? word : (currentLine + " " + word);
        tester.setString(toUtf8(testLine));
        if (tester.getLocalBounds().size.x > maxW && !currentLine.empty())
        {
            if (!result.empty()) result += "\n";
            result += currentLine;
            currentLine = word;
        }
        else
        {
            currentLine = testLine;
        }
    }
    if (!currentLine.empty())
    {
        if (!result.empty()) result += "\n";
        result += currentLine;
    }
    return result;
}
