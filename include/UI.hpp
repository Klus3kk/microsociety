#ifndef UI_HPP
#define UI_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include "UIButton.hpp"
#include "Market.hpp"
#include "MovablePanel.hpp"
#include "TimeManager.hpp"

namespace UIStyles {
    const sf::Color PanelBackground = sf::Color(45, 45, 48, 255); // Darker gray
    const sf::Color PanelBorder = sf::Color(27, 82, 40, 255);     // Lighter gray for subtle borders

    // Multicolored Buttons
    const sf::Color ButtonNormal = sf::Color(50, 50, 50, 200);  // Light green
    const sf::Color ButtonHover = sf::Color(0, 184, 148, 255);    // Vibrant green
    const sf::Color ButtonPressed = sf::Color(253, 121, 168, 255); // Soft pink for clicks

    const sf::Color TextColor = sf::Color(240, 240, 240, 255);    // Off-white for readability
}


class UI {
private:
    sf::Font font;

    // Panels
    sf::RectangleShape statusPanel;
    sf::Text statusText;

    sf::RectangleShape moneyPanel;
    sf::Text moneyText;

    sf::RectangleShape tooltipPanel;
    sf::Text tooltipText;

    sf::CircleShape clockPanel;
    sf::Text clockText;

    sf::RectangleShape centralPanel;
    sf::Text centralText;

    sf::RectangleShape marketPanel;
    sf::Text advancedMarketStatsText;
    sf::Text marketText;

    sf::RectangleShape inventoryPanel;
    sf::Text inventoryText;

    MovablePanel npcListPanel;
    MovablePanel statsPanel;
    MovablePanel npcDetailPanel;
    sf::Text npcDetailText;
    sf::Text statsText;

    UIButton statsButton;
    UIButton optionsButton;
    UIButton marketButton;
    UIButton npcButton;
    sf::RectangleShape resetButton;
    sf::RectangleShape borderButton;
    sf::RectangleShape speedSlider;
    sf::RectangleShape debugButton;
    sf::RectangleShape sliderKnob;
    bool sliderDragging = false;         // Whether the slider is being dragged
    float currentSpeed = 0.5f;  



    std::vector<std::pair<std::string, UIButton*>> npcButtons;
    std::vector<sf::Text> marketResourceTexts;

    bool showNPCList = false;
    bool showNPCDetail = false;
    bool showStatsPanel = false;
    bool showMarketPanel = false;
    bool showOptionsPanel = false;

    const float npcListWidth = 300.0f;
    const float npcListHeight = 400.0f;

    int selectedNPCIndex = -1;
    float npcListScrollOffset = 0.0f;
    const float scrollSpeed = 20.0f;

    // Animation helpers
    void applyShadow(sf::RectangleShape& shape, float offset = 3.0f);

    void populateNPCList(const std::vector<PlayerEntity>& npcs);
    void populateNPCDetails(const PlayerEntity& npc);

public:
    UI();

    // Updates
    void updateStatus(int day, const std::string& time, int iteration);
    void updateMoney(int amount);
    void updateNPCList(const std::vector<PlayerEntity>& npcs);
    void updateClock(float timeElapsed);
    void updateMarket(const std::unordered_map<std::string, float>& prices);
    void updateStats(const std::vector<PlayerEntity>& npcs, const TimeManager& timeManager);

    void showNPCDetails(const std::string& npcDetails);

    // Interaction and Rendering
    void handleButtonClicks(sf::RenderWindow& window, sf::Event& event, std::vector<PlayerEntity>& npcs, const TimeManager& timeManager, const Market& market);
    void handleNPCPanel(sf::RenderWindow& window, sf::Event& event, const std::vector<PlayerEntity>& npcs);
    void handleStatsPanel(sf::RenderWindow& window, sf::Event& event);
    void handleOptionsEvents(sf::RenderWindow& window, sf::Event& event, Game& game);

    void render(sf::RenderWindow& window, const Market& market, const std::vector<PlayerEntity>& npcs);
    void renderOptionsPanel(sf::RenderWindow& window);
    void renderMarketPanel(sf::RenderWindow& window, const Market& market);
    void drawMarketGraph(sf::RenderWindow& window, const Market& market);
    void updateMarketPanel(const Market& market);

    void setTooltipContent(const std::string& content);
    void updateTooltipPosition(const sf::RenderWindow& window);
    bool isMouseOver(sf::RenderWindow& window) const;

    void adjustLayout(sf::RenderWindow& window);
};

#endif
