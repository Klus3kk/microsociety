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

    sf::RectangleShape moneyPanel;
    sf::Text moneyText;

    sf::CircleShape clockPanel;
    sf::Text clockText;

    sf::RectangleShape centralPanel;
    sf::Text centralText;

    sf::RectangleShape marketPanel;
    sf::Text advancedMarketStatsText;
    sf::Text marketText;

    bool showOptionsPanel = false;
    sf::RectangleShape resetButton;
    sf::RectangleShape borderButton;
    sf::RectangleShape speedSlider;

    MovablePanel npcListPanel;
    MovablePanel statsPanel;
    MovablePanel npcDetailPanel;
    sf::Text npcDetailText;
    sf::Text statsText;

    // Buttons
    UIButton statsButton;
    UIButton optionsButton;
    UIButton marketButton;
    UIButton npcButton;

    std::vector<std::pair<std::string, UIButton*>> npcButtons;

    bool showNPCList = false;
    bool showNPCDetail = false;
    const float npcListWidth = 300.0f;
    const float npcListHeight = 400.0f;
    bool showStatsPanel = false;
    bool showMarketPanel = false;

    // Scroll for large NPC lists
    float npcListScrollOffset = 0.0f;
    const float scrollSpeed = 20.0f;

    // Selected NPC
    int selectedNPCIndex = -1;

    // Animation helpers
    void applyShadow(sf::RectangleShape& shape, float offset = 3.0f);

    void populateNPCList(const std::vector<PlayerEntity>& npcs);
    void populateNPCDetails(const PlayerEntity& npc);

public:
    UI();

    // Updates
    void updateStatus(int day, const std::string& time, int iteration);
    void showNPCDetails(const std::string& npcDetails);
    void updateMarket(const std::unordered_map<std::string, float>& prices);
    void updateNPCList(const std::vector<PlayerEntity>& npcs);

    void updateMoney(int amount);
    void updateClock(float timeElapsed);

    // Interaction and Rendering
    void handleButtonClicks(sf::RenderWindow& window, sf::Event& event, std::vector<PlayerEntity>& npcs, const TimeManager& timeManager);
    void updateStats(const std::vector<PlayerEntity>& npcs, const TimeManager& timeManager);

    void handleHover(sf::RenderWindow& window);
    void handleNPCPanel(sf::RenderWindow& window, sf::Event& event, const std::vector<PlayerEntity>& npcs);
    void handleStatsPanel(sf::RenderWindow& window, sf::Event& event);

    // Market functionality
    void handleMarketButton(sf::RenderWindow& window, sf::Event& event, const Market& market); // Add this declaration
    void updateMarketPanel(const Market& market);
    void drawMarketGraph(sf::RenderWindow& window, const Market& market);
    void renderMarketPanel(sf::RenderWindow& window, const Market& market);

    void render(sf::RenderWindow& window, const Market& market);

    // Tooltip interaction
    bool isMouseOver(sf::RenderWindow& window) const;
    void setTooltipContent(const std::string& content);
    void updateTooltipPosition(const sf::RenderWindow& window);

    void renderOptionsPanel(sf::RenderWindow& window);
    void handleOptionsEvents(sf::RenderWindow& window, sf::Event& event, Game& game);

    // Responsive Layout
    void adjustLayout(sf::RenderWindow& window);
};

#endif
