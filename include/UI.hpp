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

// UI Styles for consistent color themes
namespace UIStyles {
    const sf::Color PanelBackground = sf::Color(45, 45, 48, 255); // Darker gray
    const sf::Color PanelBorder = sf::Color(27, 82, 40, 255);     // Lighter gray for subtle borders

    // Multicolored Buttons
    const sf::Color ButtonNormal = sf::Color(50, 50, 50, 200);  // Dark gray
    const sf::Color ButtonHover = sf::Color(0, 184, 148, 255);    // Vibrant green on hover
    const sf::Color ButtonPressed = sf::Color(253, 121, 168, 255); // Soft pink for clicks

    const sf::Color TextColor = sf::Color(240, 240, 240, 255);    // Off-white for readability
}

class UI {
private:
    sf::Font font; // Font used in UI elements

    // **Panels for different UI elements**
    sf::RectangleShape statusPanel;     // Status display panel
    sf::Text statusText;                // Status text (day, time, etc.)

    sf::RectangleShape inventoryPanel;  // Inventory panel
    sf::Text inventoryText;             // Inventory text

    sf::RectangleShape tooltipPanel;    // Tooltip panel
    sf::Text tooltipText;               // Tooltip text

    sf::RectangleShape moneyPanel;      // Money display panel
    sf::Text moneyText;                 // Text showing money amount

    sf::CircleShape clockPanel;         // Clock panel
    sf::Text clockText;                 // Clock time text

    sf::RectangleShape centralPanel;    // Central UI panel
    sf::Text centralText;               // Central UI text

    sf::RectangleShape marketPanel;     // Market display panel
    sf::Text advancedMarketStatsText;   // Advanced market data
    sf::Text marketText;                // Market text

    // UI Components: Buttons, Sliders, etc.
    sf::RectangleShape resetButton;
    sf::RectangleShape borderButton;
    sf::RectangleShape speedSlider;

    // Movable UI Panels
    MovablePanel npcListPanel;          // NPC list UI
    MovablePanel statsPanel;            // Statistics panel
    MovablePanel npcDetailPanel;        // NPC details UI
    sf::Text npcDetailText;             // NPC details text
    sf::Text statsText;                 // Stats text

    // Interactive Buttons
    UIButton statsButton;
    UIButton optionsButton;
    UIButton marketButton;
    UIButton npcButton;
    sf::RectangleShape debugButton;
    sf::RectangleShape sliderKnob;
    bool sliderDragging = false;        // Slider dragging state
    float currentSpeed = 1.0f;          // Current game speed

    // NPC UI Elements
    std::vector<std::pair<std::string, UIButton*>> npcButtons; // NPC selection buttons
    std::vector<sf::Text> marketResourceTexts;                // Text for market resources

    // NPC List Panel Dimensions
    const float npcListWidth = 300.0f;
    const float npcListHeight = 400.0f;

    // **State Variables for UI Toggles**
    bool showNPCList = false;          // NPC list visibility
    bool showNPCDetail = false;        // NPC details visibility
    bool showStatsPanel = false;       // Statistics panel visibility
    bool showMarketPanel = false;      // Market panel visibility
    bool showOptionsPanel = false;     // Options panel visibility

    // **Scroll Offset for large NPC lists**
    float npcListScrollOffset = 0.0f;
    const float scrollSpeed = 20.0f;

    // **Currently Selected NPC**
    int selectedNPCIndex = -1;

    // **Helper Functions**
    void applyShadow(sf::RectangleShape& shape, float offset = 3.0f);
    void populateNPCList(const std::vector<NPCEntity>& npcs);
    void populateNPCDetails(const NPCEntity& npc);

public:
    // **Constructor**
    UI();

    // **Update Methods**
    void updateStatus(int day, const std::string& time, int iteration);
    void showNPCDetails(const std::string& npcDetails);
    void updateMarket(const std::unordered_map<std::string, float>& prices);
    void updateNPCList(const std::vector<NPCEntity>& npcs);
    void updateMoney(int amount);
    void updateClock(float timeElapsed);
    void updateStats(const std::vector<NPCEntity>& npcs, const TimeManager& timeManager);

    // **User Interaction & Rendering**
    void handleButtonClicks(sf::RenderWindow& window, sf::Event& event, std::vector<NPCEntity>& npcs, const TimeManager& timeManager, const Market& market);
    void handleNPCPanel(sf::RenderWindow& window, sf::Event& event, const std::vector<NPCEntity>& npcs);
    void handleStatsPanel(sf::RenderWindow& window, sf::Event& event);
    void handleOptionsEvents(sf::RenderWindow& window, sf::Event& event, Game& game);

    void render(sf::RenderWindow& window, const Market& market, const std::vector<NPCEntity>& npcs);
    void renderOptionsPanel(sf::RenderWindow& window);
    void renderMarketPanel(sf::RenderWindow& window, const Market& market);
    void drawMarketGraph(sf::RenderWindow& window, const Market& market);
    void updateMarketPanel(const Market& market);

    // **Tooltip Handling**
    void handleHover(sf::RenderWindow& window);
    bool isMouseOver(sf::RenderWindow& window) const;
    void setTooltipContent(const std::string& content);
    void updateTooltipPosition(const sf::RenderWindow& window);

    // **NPC UI Management**
    void updateNPCEntityList(const std::vector<NPCEntity>& npcs);
    void handleNPCEntityPanel(sf::RenderWindow& window, sf::Event& event, const std::vector<NPCEntity>& npcs);
    void updateSliderValue(float newValue, Game& game);

    // **UI Responsiveness**
    void adjustLayout(sf::RenderWindow& window);
    void updateAll(const std::vector<NPCEntity>& npcs, const Market& market, const TimeManager& timeManager);
    void hideAllPanels();
    void enableNPCListScrolling(sf::Event& event);
    void resetMarketGraph();
};

#endif
