#include "SFML/Graphics.hpp"

class Textbox
{
public:
    sf::RectangleShape box;
    sf::Text text;
    bool isSelected = false;
    std::string content;
    Textbox(const sf::Vector2f &pos, const sf::Vector2f &size, sf::Font &font, unsigned int characterSize = 20)
    {
        box.setPosition(pos);
        box.setSize(size);
        box.setFillColor(sf::Color::White);
        box.setOutlineColor(sf::Color::Black);
        box.setOutlineThickness(2);
        text.setFont(font);
        text.setPosition(pos.x + 5, pos.y + 5);
        text.setCharacterSize(characterSize);
        text.setFillColor(sf::Color::Black);
    }
    void draw(sf::RenderWindow &window)
    {
        window.draw(box);
        window.draw(text);
    }
    void updateText()
    {
        text.setString(content);
    }
};