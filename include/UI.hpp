#ifndef UI_HPP
#define UI_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include "UIButton.hpp"

class UI {
private:
    sf::Font font;

    // Panels
    sf::RectangleShape statusPanel;
    sf::Text statusText;

    sf::RectangleShape inventoryPanel;
    sf::Text inventoryText;

    sf::RectangleShape tooltipPanel;
    sf::Text tooltipText;

    // Buttons
    UIButton npcButton;
    UIButton statsButton;
    UIButton optionsButton;

public:
    UI();

    void updateStatus(int day, const std::string& time, int npcCount, int totalMoney, const std::unordered_map<std::string, int>& allResources);
    void handleButtonClicks(sf::RenderWindow& window, sf::Event& event);
    void handleHover(sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
    void setTooltipContent(const std::string& content);
};

#endif 