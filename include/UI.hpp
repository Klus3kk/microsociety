#ifndef UI_HPP
#define UI_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
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

    sf::RectangleShape npcListPanel;
    std::vector<std::pair<std::string, UIButton>> npcButtons;

    sf::RectangleShape npcDetailPanel;
    sf::Text npcDetailText;

    sf::RectangleShape marketPanel;
    sf::Text marketText;

    sf::RectangleShape tooltipPanel;
    sf::Text tooltipText;

    // Buttons
    UIButton statsButton;
    UIButton optionsButton;

    // Selected NPC
    int selectedNPCIndex = -1;

    // Animation helpers
    void applyShadow(sf::RectangleShape& shape, float offset = 3.0f);

public:
    UI();
    void updateStatus(int day, const std::string& time, int npcCount, int totalMoney, const std::unordered_map<std::string, int>& allResources);
    void updateNPCList(const std::vector<std::string>& npcNames);
    void showNPCDetails(const std::string& npcDetails);
    void updateMarket(const std::unordered_map<std::string, float>& prices);

    void handleButtonClicks(sf::RenderWindow& window, sf::Event& event);
    void handleHover(sf::RenderWindow& window);

    void render(sf::RenderWindow& window);

    // Tooltip interaction
    bool isMouseOver(sf::RenderWindow& window) const;
    void setTooltipContent(const std::string& content);
    void updateTooltipPosition(const sf::RenderWindow& window);
};

#endif
