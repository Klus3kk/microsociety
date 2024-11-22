#ifndef UI_HPP
#define UI_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include "UIButton.hpp"
#include "Entity.hpp"

class UI {
private:
    sf::Font font;

    // Panels
    sf::RectangleShape statusPanel;
    sf::Text statusText;

    sf::RectangleShape inventoryPanel;
    sf::Text inventoryText;

    sf::RectangleShape npcListPanel;  // List of NPCs
    std::vector<std::pair<std::string, UIButton>> npcButtons; // NPC names and buttons

    sf::RectangleShape npcDetailPanel;  // Detailed NPC stats
    sf::Text npcDetailText;

    sf::RectangleShape tooltipPanel;
    sf::Text tooltipText;

    // Buttons
    UIButton statsButton;
    UIButton optionsButton;

    // Selected NPC
    int selectedNPCIndex = -1; // -1 means no NPC is selected

public:
    UI();
    void updateStatus(int day, const std::string& time, int npcCount, int totalMoney, const std::unordered_map<std::string, int>& allResources);
    void updateNPCList(const std::vector<std::string>& npcNames); // Update NPC list
    void showNPCDetails(const std::string& npcDetails); // Show selected NPC details

    void handleButtonClicks(sf::RenderWindow& window, sf::Event& event);
    void handleHover(sf::RenderWindow& window);

    void render(sf::RenderWindow& window);
    
    // Check if the mouse is over the UI
    bool isMouseOver(sf::RenderWindow& window) const;

    // Set tooltip text content
    void setTooltipContent(const std::string& content);
};

#endif
