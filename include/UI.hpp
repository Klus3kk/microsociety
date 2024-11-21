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

    // Update status with relevant information
    void updateStatus(int day, const std::string& time, int npcCount, int totalMoney, const std::unordered_map<std::string, int>& allResources);

    // Handle button interactions
    void handleButtonClicks(sf::RenderWindow& window, sf::Event& event);
    void handleHover(sf::RenderWindow& window);

    // Render UI
    void render(sf::RenderWindow& window);

    // Set tooltip text content
    void setTooltipContent(const std::string& content);
};

#endif 