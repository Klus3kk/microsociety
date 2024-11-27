#ifndef UI_HPP
#define UI_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include "UIButton.hpp"
#include "Market.hpp"
#include "MovablePanel.hpp"

class UI {
private:
    sf::Font font;

    // Panels
    sf::RectangleShape statusPanel;
    sf::Text statusText;

    sf::RectangleShape inventoryPanel;
    sf::Text inventoryText;

    sf::RectangleShape marketPanel;
    sf::Text marketText;

    sf::RectangleShape tooltipPanel;
    sf::Text tooltipText;

    sf::RectangleShape moneyPanel;
    sf::Text moneyText;

    sf::CircleShape clockPanel;
    sf::Text clockText;

    sf::RectangleShape centralPanel;
    sf::Text centralText;

    MovablePanel npcListPanel;
    MovablePanel npcDetailPanel;
    sf::Text npcDetailText; // Declare this for NPC details

    // Buttons
    UIButton statsButton;
    UIButton optionsButton;
    UIButton marketButton;
    UIButton npcButton;

    std::vector<std::pair<std::string, UIButton>> npcButtons;

    bool showNPCList = false;  // Toggles the NPC list panel
    bool showNPCDetail = false; // Toggles the NPC detail panel

    // Scroll for large NPC lists
    float npcListScrollOffset = 0.0f;
    const float scrollSpeed = 20.0f;

    // Helper to display NPC details
    void displayNPCDetails(const std::string& npcDetails);

    // Selected NPC
    int selectedNPCIndex = -1;

    // Animation helpers
    void applyShadow(sf::RectangleShape& shape, float offset = 3.0f);

public:
    UI();

    // Updates
    void updateStatus(int day, const std::string& time, int iteration);
    void updateNPCList(const std::vector<std::string>& npcNames);
    void showNPCDetails(const std::string& npcDetails);
    void updateMarket(const std::unordered_map<std::string, float>& prices);

    void updateMoney(int amount);
    void updateClock(float timeElapsed);

    // Interaction and Rendering
    void handleButtonClicks(sf::RenderWindow& window, sf::Event& event);
    void handleHover(sf::RenderWindow& window);
    void render(sf::RenderWindow& window, const Market& market);
    void drawPriceTrends(sf::RenderWindow& window, const Market& market);

    // Tooltip interaction
    bool isMouseOver(sf::RenderWindow& window) const;
    void setTooltipContent(const std::string& content);
    void updateTooltipPosition(const sf::RenderWindow& window);

    // Responsive Layout
    void adjustLayout(sf::RenderWindow& window);
};

#endif
