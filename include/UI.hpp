#ifndef UI_HPP
#define UI_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

class UI {
private:
    sf::Font font;

    // Status Panel
    sf::RectangleShape statusPanel;
    sf::Text statusText;

    // Tooltip Panel
    sf::RectangleShape tooltipPanel;
    sf::Text tooltipText;

    // Buttons
    void createButtons();
    void updateTooltipPosition(const sf::Vector2i& mousePosition);

public:
    UI();
    void updateStatus(int day, const std::string& time, int npcCount, int totalMoney);
    void handleButtonClicks(sf::RenderWindow& window, sf::Event& event);
    void handleHover(sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
    void setTooltipContent(const std::string& content);
};

#endif 
