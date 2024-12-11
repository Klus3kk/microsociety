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

<<<<<<< HEAD

=======
namespace UIStyles {
    const sf::Color PanelBackground = sf::Color(45, 45, 48, 255); // Darker gray
    const sf::Color PanelBorder = sf::Color(27, 82, 40, 255);     // Lighter gray for subtle borders

    // Multicolored Buttons
    const sf::Color ButtonNormal = sf::Color(50, 50, 50, 200);  // Light green
    const sf::Color ButtonHover = sf::Color(0, 184, 148, 255);    // Vibrant green
    const sf::Color ButtonPressed = sf::Color(253, 121, 168, 255); // Soft pink for clicks

    const sf::Color TextColor = sf::Color(240, 240, 240, 255);    // Off-white for readability
}
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db


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
<<<<<<< HEAD
=======
    sf::RectangleShape resetButton;
    sf::RectangleShape borderButton;
    sf::RectangleShape speedSlider;
    sf::RectangleShape debugButton;
    sf::RectangleShape sliderKnob;
    bool sliderDragging = false;         // Whether the slider is being dragged
    float currentSpeed = 0.5f;  


>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db

    std::vector<std::pair<std::string, UIButton*>> npcButtons;
    std::vector<sf::Text> marketResourceTexts;

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
<<<<<<< HEAD
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
=======
    void handleButtonClicks(sf::RenderWindow& window, sf::Event& event, std::vector<PlayerEntity>& npcs, const TimeManager& timeManager, const Market& market);
    void handleNPCPanel(sf::RenderWindow& window, sf::Event& event, const std::vector<PlayerEntity>& npcs);
    void handleStatsPanel(sf::RenderWindow& window, sf::Event& event);
    void handleOptionsEvents(sf::RenderWindow& window, sf::Event& event, Game& game);

    void render(sf::RenderWindow& window, const Market& market, const std::vector<PlayerEntity>& npcs);
    void renderOptionsPanel(sf::RenderWindow& window);
    void renderMarketPanel(sf::RenderWindow& window, const Market& market);
    void drawMarketGraph(sf::RenderWindow& window, const Market& market);
    void updateMarketPanel(const Market& market);
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db

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
