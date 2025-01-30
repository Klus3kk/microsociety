
#include "UI.hpp"
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include "Game.hpp"
UI::UI()
    : npcListPanel(npcListWidth, npcListHeight, "NPC List"),
      npcDetailPanel(300, 500, "NPC Details"),
      statsPanel(500, 400, ""),
      marketPanel(sf::Vector2f(600, 400)) {
    if (!font.loadFromFile("../assets/fonts/font.ttf")) {
        throw std::runtime_error("Failed to load font!");
    }

    // Market Panel
    marketPanel.setSize({600, 400});
    marketPanel.setPosition(120, 120);
    marketPanel.setFillColor(UIStyles::PanelBackground);
    applyShadow(marketPanel);

    // Advanced Market Stats Text
    advancedMarketStatsText.setFont(font);
    advancedMarketStatsText.setCharacterSize(14);
    advancedMarketStatsText.setFillColor(sf::Color::White);

    // NPC List Panel
    npcListPanel.setPosition(50, 100);
    npcListPanel.setSize(npcListWidth, npcListHeight);
    npcDetailText.setFont(font);
    npcDetailText.setCharacterSize(16);
    npcDetailText.setFillColor(UIStyles::TextColor);
    npcDetailText.setPosition(npcDetailPanel.getBounds().left + 20, npcDetailPanel.getBounds().top + 20);

    // Stats Text
    statsText.setFont(font);
    statsText.setCharacterSize(16);
    statsText.setFillColor(UIStyles::TextColor);
    statsText.setPosition(statsPanel.getBounds().left + 20, statsPanel.getBounds().top + 20);

    // Money Panel
    moneyPanel.setSize({100, 50});
    moneyPanel.setPosition(20, 20);
    moneyPanel.setFillColor(sf::Color(30, 30, 30));
    applyShadow(moneyPanel);

    moneyText.setFont(font);
    moneyText.setCharacterSize(20);
    moneyText.setFillColor(UIStyles::TextColor);
    moneyText.setPosition(moneyPanel.getPosition().x + 10, moneyPanel.getPosition().y + 10);

    // Status Panel
    statusPanel.setSize({200, 80});
    statusPanel.setPosition(300, 20); // Centered horizontally
    statusPanel.setFillColor(sf::Color(55, 55, 55));
    applyShadow(statusPanel);

    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setFillColor(UIStyles::TextColor);
    statusText.setPosition(statusPanel.getPosition().x + 10, statusPanel.getPosition().y + 10);

    
    // Buttons (NPC, Stats, Market, Options)
    float buttonWidth = 120.0f;
    float buttonHeight = 40.0f;
    float spacing = 20.0f;
    float startX = 50.0f; // Left margin
    float startY = 500.0f; // Bottom

    npcButton.setProperties(startX, startY, buttonWidth, buttonHeight, "NPC", font);
    statsButton.setProperties(startX + buttonWidth + spacing, startY, buttonWidth, buttonHeight, "STATS", font);
    marketButton.setProperties(startX + 2 * (buttonWidth + spacing), startY, buttonWidth, buttonHeight, "MARKET", font);
    optionsButton.setProperties(startX + 3 * (buttonWidth + spacing), startY, buttonWidth, buttonHeight, "OPTIONS", font);

    // Set Button Colors
    npcButton.setColors(UIStyles::ButtonNormal, UIStyles::ButtonHover, sf::Color(160, 160, 160, 204), UIStyles::TextColor);
    statsButton.setColors(UIStyles::ButtonNormal, UIStyles::ButtonHover, sf::Color(160, 160, 160, 204), UIStyles::TextColor);
    marketButton.setColors(UIStyles::ButtonNormal, UIStyles::ButtonHover, sf::Color(160, 160, 160, 204), UIStyles::TextColor);
    optionsButton.setColors(UIStyles::ButtonNormal, UIStyles::ButtonHover, sf::Color(160, 160, 160, 204), UIStyles::TextColor);
}



void UI::applyShadow(sf::RectangleShape& shape, float offset) {
    shape.setOutlineThickness(offset);
    shape.setOutlineColor(sf::Color(0, 0, 0, 100));
}

// Update Money
void UI::updateMoney(int amount) {
    moneyText.setString("$ " + std::to_string(amount));
}

// Update Clock
void UI::updateClock(float timeElapsed) {
    clockText.setString("Clock\n" + std::to_string(static_cast<int>(timeElapsed)) + "s");
}


void UI::updateStatus(int day, const std::string& time, int iteration) {
    std::ostringstream status;
    status << "Day: " << day << "\n"
           << "Time: " << time << "\n"
           << "Iteration: " << (iteration + 1); 

    statusText.setString(status.str());
}


void UI::resetMarketGraph() {
    marketResourceTexts.clear(); // Clear previous resource texts
}


void UI::enableNPCListScrolling(sf::Event& event) {
    if (!showNPCList) return; // Ensure NPC list is open

    if (event.type == sf::Event::MouseWheelScrolled) {
        float scrollAmount = event.mouseWheelScroll.delta * -30.0f; // Adjust sensitivity
        npcListScrollOffset += scrollAmount;

        // Constrain scrolling limits
        float minOffset = 0;
        float maxOffset = std::max(0.0f, static_cast<float>(npcButtons.size() * 50 - npcListPanel.getBounds().height));
        npcListScrollOffset = std::clamp(npcListScrollOffset, minOffset, maxOffset);

        // Update button positions based on scroll offset
        float buttonY = npcListPanel.getBounds().top + 20 - npcListScrollOffset;
        for (auto& [_, button] : npcButtons) {
            button->setProperties(
                button->getPosition().x,
                buttonY,
                button->getSize().x,
                button->getSize().y,
                button->getText(),
                font
            );
            buttonY += 50;
        }
    }
}



void UI::updateNPCList(const std::vector<NPCEntity>& npcs) {
    npcButtons.clear();
    npcListPanel.clearChildren(); // Remove old buttons

    float panelWidth = 300.0f;  // Reduce panel width
    float buttonWidth = panelWidth - 40.0f; // Buttons smaller than panel
    float buttonHeight = 45.0f;  // Slightly reduced height
    float spacing = 5.0f;        // Keep minimal spacing
    float titlePadding = 50.0f;  // Ensure title doesn't overlap
    float bottomPadding = 50.0f; // Extra space at the bottom

    // Ensure total height fits properly
    float calculatedHeight = npcs.size() * (buttonHeight + spacing) + titlePadding + bottomPadding;
    float minHeight = 300.0f;
    float maxHeight = 600.0f;
    float newHeight = std::clamp(calculatedHeight, minHeight, maxHeight);

    // Set panel size correctly
    npcListPanel.setSize(panelWidth, newHeight);
    npcListPanel.setPosition(50, 100); // Ensure position

    if (!showNPCList) return; 

    // Adjust button positions properly
    float startX = npcListPanel.getBounds().left + 20; // Keep margin
    float startY = npcListPanel.getBounds().top + titlePadding;

    for (size_t i = 0; i < npcs.size(); ++i) {
        UIButton* button = new UIButton();
        button->setProperties(
            startX,
            startY,
            buttonWidth,  // Reduced width
            buttonHeight,
            npcs[i].getName(),
            font
        );
        button->setColors(
            sf::Color(80, 80, 150, 204),
            sf::Color(100, 100, 200, 204),
            sf::Color(60, 60, 120, 204),
            sf::Color::White
        );

        npcListPanel.addChild(button);
        npcButtons.emplace_back(npcs[i].getName(), button);
        startY += buttonHeight + spacing;  // Stack properly
    }
}

void UI::updateMarket(const std::unordered_map<std::string, float>& prices) {
    std::ostringstream marketStream;
    marketStream << "Market Prices:\n";
    for (const auto& [item, price] : prices) {
        marketStream << "- " << item << ": $" << price << "\n";
    }
    marketText.setString(marketStream.str());
}

void UI::populateNPCDetails(const NPCEntity& npc) {
    // Update the text content for NPC details
    std::ostringstream details;
    details << "Name: " << npc.getName() << "\n"
            << "Health: " << npc.getHealth() << "\n"
            // << "Hunger: " << npc.getHunger() << "\n"
            << "Energy: " << static_cast<int>(npc.getEnergyPercentage() * 100) << "%\n"
            << "Speed: " << npc.getBaseSpeed() << "\n"
            // << "Strength: " << npc.getStrength() << "\n"
            << "Money: $" << npc.getMoney() << "\n"
            << "\nInventory:\n";

    for (const auto& [item, quantity] : npc.getInventory()) {
        details << "- " << item << ": " << quantity << "\n";
    }

    // Update the detail text for the panel
    npcDetailText.setString(details.str());

    // Center text within the panel
    npcDetailText.setPosition(
        npcDetailPanel.getBounds().left + 20, // Padding
        npcDetailPanel.getBounds().top + 50  // Start below the title
    );
}

void UI::showNPCDetails(const std::string& npcDetails) {
    npcDetailPanel.setSize(400, 800);
    npcDetailPanel.setTitle("NPC Details");
    npcDetailText.setFont(font);
    npcDetailText.setCharacterSize(16);
    npcDetailText.setFillColor(sf::Color::White);
    npcDetailPanel.setPosition((sf::VideoMode::getDesktopMode().width - 400) / 2, 120);
    npcDetailText.setString(npcDetails);
}


void UI::hideAllPanels() {
    showNPCList = false;
    showNPCDetail = false;
    showStatsPanel = false;
    showMarketPanel = false;
    showOptionsPanel = false;
}

void UI::handleButtonClicks(sf::RenderWindow& window, sf::Event& event, std::vector<NPCEntity>& npcs, const TimeManager& timeManager, const Market& market) {
    // NPC Button - Show NPC List, Hide Others
    if (npcButton.isClicked(window, event)) {
        bool wasVisible = showNPCList;
        hideAllPanels();  // Hide all panels first
        showNPCList = !wasVisible; // Toggle NPC list
    }

    // Market Button - Show Market Panel, Hide Others
    if (marketButton.isClicked(window, event)) {
        bool wasVisible = showMarketPanel;
        hideAllPanels();
        showMarketPanel = !wasVisible;
        if (showMarketPanel) {
            updateMarketPanel(market);  // Ensure real-time update
        }
    }

    // Stats Button - Show Stats Panel, Hide Others
    if (statsButton.isClicked(window, event)) {
        bool wasVisible = showStatsPanel;
        hideAllPanels();
        showStatsPanel = !wasVisible;
        if (showStatsPanel) {
            updateStats(npcs, timeManager);  // Ensure real-time update
        }
    }

    // Options Button - Show Options Panel, Hide Others
    if (optionsButton.isClicked(window, event)) {
        bool wasVisible = showOptionsPanel;
        hideAllPanels();
        showOptionsPanel = !wasVisible;
    }

    // Handle NPC List Clicks (Show Details, Hide List)
    if (showNPCList) {
        for (size_t i = 0; i < npcButtons.size(); ++i) {
            if (npcButtons[i].second->isClicked(window, event)) {
                selectedNPCIndex = static_cast<int>(i);
                populateNPCDetails(npcs[selectedNPCIndex]);  // Update UI
                showNPCDetail = true;
                showNPCList = false;
                break;
            }
        }
    }
}

void UI::handleStatsPanel(sf::RenderWindow& window, sf::Event& event) {
    if (showStatsPanel) {
        statsPanel.handleEvent(window, event);
    }
}

void UI::updateStats(const std::vector<NPCEntity>& npcs, const TimeManager& timeManager) {
    std::ostringstream statsStream;
    
    // Time Details
    statsStream << "Day: " << timeManager.getCurrentDay() << "\n";
    statsStream << "Time: " << timeManager.getFormattedTime() << "\n";
    statsStream << "Iteration: " << timeManager.getSocietyIteration() << "\n\n";

    // NPC Stats Section
    statsStream << "NPC Stats:\n";
    statsStream << "  Total NPCs: " << npcs.size() << "\n";

    float totalHealth = 0, totalEnergy = 0, totalHunger = 0;
    for (const auto& npc : npcs) {
        totalHealth += npc.getHealth();
        totalEnergy += npc.getEnergy();
        totalHunger += npc.getHunger();
    }

    // Averages
    statsStream << "  Avg. Health: " << (npcs.empty() ? 0 : totalHealth / npcs.size()) << "\n";
    statsStream << "  Avg. Energy: " << (npcs.empty() ? 0 : totalEnergy / npcs.size()) << "\n";
    // statsStream << "  Avg. Hunger: " << (npcs.empty() ? 0 : totalHunger / npcs.size()) << "\n\n";

    // Resource Stats
    statsStream << "Resource Stats:\n";
    std::unordered_map<std::string, int> totalResources;
    for (const auto& npc : npcs) {
        for (const auto& [item, quantity] : npc.getInventory()) {
            totalResources[item] += quantity;
        }
    }
    for (const auto& [item, total] : totalResources) {
        statsStream << "  " << item << ": " << total << "\n";
    }

    // Update the stats text
    statsText.setString(statsStream.str());

    // **Center the Text in statsPanel**
    float textWidth = statsText.getLocalBounds().width;
    float panelWidth = statsPanel.getBounds().width;

    statsText.setPosition(
        statsPanel.getBounds().left + (panelWidth - textWidth) / 2,  // Center horizontally
        statsPanel.getBounds().top + 50  // Keep padding
    );
}


void UI::handleHover(sf::RenderWindow& window) {
    npcButton.handleHover(window);
    statsButton.handleHover(window);
    marketButton.handleHover(window);
    optionsButton.handleHover(window);

    if (showNPCList) {
        for (auto& [_, button] : npcButtons) {
            button->handleHover(window);
        }
    }   
}

void UI::populateNPCList(const std::vector<NPCEntity>& npcs) {
    npcListPanel.setSize(400, 800);
    // npcListPanel.setTitle("");
    npcListPanel.clearChildren();

    float startY = npcListPanel.getBounds().top + 50; // Start below the title
    float buttonSpacing = 5.0f;

    for (size_t i = 0; i < npcs.size(); ++i) {
        UIButton* button = new UIButton();
        button->setProperties(
            npcListPanel.getBounds().left + 20, // Padding
            startY,
            npcListPanel.getBounds().width - 40, // Width with padding
            40,
            npcs[i].getName(),
            font
        );
        button->setColors(
            UIStyles::ButtonNormal,
            UIStyles::ButtonHover,
            UIStyles::ButtonPressed,
            UIStyles::TextColor
        );

        npcListPanel.addChild(button);
        npcButtons.emplace_back(npcs[i].getName(), button);
        startY += 50 + buttonSpacing; // Button height + spacing
    }
}

void UI::handleNPCPanel(sf::RenderWindow& window, sf::Event& event, const std::vector<NPCEntity>& npcs) {
    npcListPanel.handleEvent(window, event);

    // Handle scrolling
    if (event.type == sf::Event::MouseWheelScrolled && showNPCList) {
        enableNPCListScrolling(event);
    }

    // Handle clicks on NPC buttons
    for (size_t i = 0; i < npcButtons.size(); ++i) {
        if (npcButtons[i].second->isClicked(window, event)) {
            selectedNPCIndex = static_cast<int>(i);
            populateNPCDetails(npcs[selectedNPCIndex]);
            showNPCDetail = true;
            showNPCList = false; // Hide list when showing details
            break;
        }
    }
}

void UI::render(sf::RenderWindow& window, const Market& market, const std::vector<NPCEntity>& npcs) {
    // Top Panels
    window.draw(moneyPanel);
    window.draw(moneyText);
    window.draw(statusPanel);
    window.draw(statusText);

    // Bottom Buttons
    npcButton.draw(window);
    statsButton.draw(window);
    marketButton.draw(window);
    optionsButton.draw(window);

    // Draw tooltip if applicable
    if (isMouseOver(window)) {
        window.draw(tooltipPanel);
        window.draw(tooltipText);
    }

    if (showNPCList) {
        npcListPanel.render(window);
        for (size_t i = 0; i < npcButtons.size(); ++i) {
            npcButtons[i].second->draw(window);
        }
    }


    // Render NPC Details
    if (showNPCDetail && selectedNPCIndex >= 0 && selectedNPCIndex < static_cast<int>(npcs.size())) {
        populateNPCDetails(npcs[selectedNPCIndex]); // Dynamic update
        npcDetailPanel.render(window);
        window.draw(npcDetailText);
    }

    if (showNPCDetail) {
        npcDetailPanel.render(window);
        window.draw(npcDetailText);
    }

    if (showStatsPanel) {
        statsPanel.render(window);
        window.draw(statsText);

        sf::Text title("Simulation Stats", font, 20);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold);
        title.setPosition(
            statsPanel.getBounds().left + (statsPanel.getBounds().width - title.getGlobalBounds().width) / 2,
            statsPanel.getBounds().top + 10
        );
        window.draw(title);
    }

    if (showMarketPanel) {
        renderMarketPanel(window, market);
    }


    if (showOptionsPanel) {
        renderOptionsPanel(window);
    }

}

void UI::setTooltipContent(const std::string& content) {
    tooltipText.setString(content);
    tooltipPanel.setSize(sf::Vector2f(tooltipText.getLocalBounds().width + 20, tooltipText.getLocalBounds().height + 20));
}

void UI::updateTooltipPosition(const sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    tooltipPanel.setPosition(static_cast<float>(mousePos.x + 10), static_cast<float>(mousePos.y + 10));
}

bool UI::isMouseOver(sf::RenderWindow& window) const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mouseFloatPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return statusPanel.getGlobalBounds().contains(mouseFloatPos) ||
           inventoryPanel.getGlobalBounds().contains(mouseFloatPos) ||
           statsButton.isMouseOver(window) ||
           optionsButton.isMouseOver(window);
}

void UI::adjustLayout(sf::RenderWindow& window) {
    sf::Vector2u size = window.getSize();


    // NPC List Panel
    npcListPanel.setPosition(
        (size.x - npcListPanel.getBounds().width) / 2 - 220, // Offset left
        size.y * 0.1f
    );

    // NPC Details Panel
    npcDetailPanel.setPosition(
        (size.x - npcDetailPanel.getBounds().width) / 2 + 220, // Offset right
        size.y * 0.18f
    );

    npcDetailText.setPosition(
        npcDetailPanel.getBounds().left + 20,
        npcDetailPanel.getBounds().top + 50
    );

    // Center the Stats Panel
    statsPanel.setPosition(
        (size.x - statsPanel.getBounds().width) / 2, // Center horizontally
        (size.y - statsPanel.getBounds().height) / 2 // Center vertically
    );

    // Adjust Stats Text Position
    statsText.setPosition(
        statsPanel.getBounds().left + 20, // Padding from the left
        statsPanel.getBounds().top + 20  // Padding from the top
    );

    marketPanel.setPosition(
        (size.x - marketPanel.getSize().x) / 2,
        (size.y - marketPanel.getSize().y) / 2
    );


    // Money Panel
    moneyPanel.setPosition(size.x * 0.02f, size.y * 0.02f);
    moneyText.setPosition(moneyPanel.getPosition().x + 10, moneyPanel.getPosition().y + 5);

    // Status Panel
    statusPanel.setPosition((size.x - statusPanel.getSize().x) / 2, size.y * 0.02f);
    statusText.setPosition(statusPanel.getPosition().x + 10, statusPanel.getPosition().y + 5);

    // Clock Panel
    clockPanel.setPosition(size.x - clockPanel.getRadius() * 2.5f, size.y * 0.02f);
    clockText.setPosition(clockPanel.getPosition().x + 15, clockPanel.getPosition().y + 15);

    // Central Panel (Dynamic Content)
    centralPanel.setSize({size.x * 0.6f, size.y * 0.5f});
    centralPanel.setPosition((size.x - centralPanel.getSize().x) / 2, size.y * 0.3f);
    centralText.setPosition(centralPanel.getPosition().x + 10, centralPanel.getPosition().y + 10);

    // Bottom Buttons
    float buttonWidth = size.x * 0.2f;
    float buttonHeight = size.y * 0.08f;
    float spacing = size.x * 0.02f;
    float startY = size.y - buttonHeight - 20;

    npcButton.setProperties(size.x * 0.1f, startY, buttonWidth, buttonHeight, "NPC", font);
    statsButton.setProperties(size.x * 0.3f, startY, buttonWidth, buttonHeight, "STATS", font);
    marketButton.setProperties(size.x * 0.5f, startY, buttonWidth, buttonHeight, "MARKET", font);
    optionsButton.setProperties(size.x * 0.7f, startY, buttonWidth, buttonHeight, "OPTIONS", font);
}




void UI::updateMarketPanel(const Market& market) {
    marketResourceTexts.clear(); // Clear old entries

    float startX = marketPanel.getPosition().x + 20;
    float startY = marketPanel.getPosition().y + 60;
    float columnWidth = 180.0f;  // Ensures proper spacing
    float rowSpacing = 100.0f;  

    advancedMarketStatsText.setString("Market Prices and Stats");
    advancedMarketStatsText.setPosition(
        marketPanel.getPosition().x + (marketPanel.getSize().x - advancedMarketStatsText.getGlobalBounds().width) / 2,
        marketPanel.getPosition().y + 10
    );

    int index = 0;
    for (const auto& [resource, price] : market.getPrices()) {
        std::ostringstream resourceStats;
        resourceStats << resource << ":\n"
                      << "  Price: $" << price << "\n";
                    //   << "  Buy: " << market.getBuyTransactions(resource) << "\n"
                    //   << "  Sell: " << market.getSellTransactions(resource) << "\n"
                    //   << "  Revenue: $" << market.getRevenue(resource) << "\n"
                    //   << "  Expenditure: $" << market.getExpenditure(resource) << "\n";
                    //   << "  Volatility: " << market.calculateVolatility(resource) << "\n";

        sf::Text resourceText(resourceStats.str(), font, 16);
        resourceText.setFillColor(sf::Color::White);
        
        float col = index % 3;
        float row = index / 3;
        resourceText.setPosition(startX + col * columnWidth, startY + row * rowSpacing);
        
        marketResourceTexts.push_back(resourceText);
        index++;
    }
}

void UI::renderOptionsPanel(sf::RenderWindow& window) {
    if (!showOptionsPanel) return;

    // Get window dimensions for dynamic centering
    sf::Vector2u windowSize = window.getSize();

    // Background Panel
    sf::RectangleShape optionsPanel;
    optionsPanel.setSize({600, 400});
    optionsPanel.setFillColor(sf::Color(50, 50, 50, 200));
    optionsPanel.setOutlineThickness(2);
    optionsPanel.setOutlineColor(sf::Color(150, 150, 150)); // Light gray border

    // Center the panel dynamically
    optionsPanel.setPosition(
        (windowSize.x - optionsPanel.getSize().x) / 2.0f,
        (windowSize.y - optionsPanel.getSize().y) / 2.0f
    );

    // Title
    sf::Text title("Options", font, 24);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition(
        optionsPanel.getPosition().x + (optionsPanel.getSize().x - title.getGlobalBounds().width) / 2.0f,
        optionsPanel.getPosition().y + 20
    );

    // Button layout parameters
    float buttonWidth = 400.0f;
    float buttonHeight = 40.0f;
    float spacing = 20.0f;

    // Start positions for buttons
    float startX = optionsPanel.getPosition().x + (optionsPanel.getSize().x - buttonWidth) / 2.0f;
    float startY = optionsPanel.getPosition().y + 80.0f;

    // Reset Simulation Button
    sf::RectangleShape resetButton({buttonWidth, buttonHeight});
    resetButton.setFillColor(sf::Color(100, 100, 100));
    resetButton.setPosition(startX, startY);
    sf::Text resetText("Reset Simulation", font, 18);
    resetText.setFillColor(sf::Color::White);
    resetText.setPosition(
        resetButton.getPosition().x + (resetButton.getSize().x - resetText.getGlobalBounds().width) / 2.0f,
        resetButton.getPosition().y + (resetButton.getSize().y - resetText.getGlobalBounds().height) / 2.0f
    );

    // Toggle Tile Borders Button
    sf::RectangleShape borderButton({buttonWidth, buttonHeight});
    borderButton.setFillColor(sf::Color(100, 100, 100));
    borderButton.setPosition(startX, startY + buttonHeight + spacing);
    sf::Text borderText("Toggle Tile Borders", font, 18);
    borderText.setFillColor(sf::Color::White);
    borderText.setPosition(
        borderButton.getPosition().x + (borderButton.getSize().x - borderText.getGlobalBounds().width) / 2.0f,
        borderButton.getPosition().y + (borderButton.getSize().y - borderText.getGlobalBounds().height) / 2.0f
    );

    // Simulation Speed Slider
    sf::Text speedText("Simulation Speed", font, 18);
    speedText.setFillColor(sf::Color::White);
    speedText.setPosition(startX, startY + 2 * (buttonHeight + spacing));
    sf::RectangleShape speedSlider({buttonWidth, 10});
    speedSlider.setFillColor(sf::Color(150, 150, 150));
    speedSlider.setPosition(startX, startY + 2 * (buttonHeight + spacing) + 30);
    sf::RectangleShape sliderKnob({10, 20});
    sliderKnob.setFillColor(sf::Color::White);
    sliderKnob.setPosition(
        speedSlider.getPosition().x + currentSpeed * (speedSlider.getSize().x - sliderKnob.getSize().x),
        speedSlider.getPosition().y - 5
    );

    // Toggle Debug Console Button
    sf::RectangleShape debugButton({buttonWidth, buttonHeight});
    debugButton.setFillColor(sf::Color(100, 100, 100));
    debugButton.setPosition(startX, startY + 3 * (buttonHeight + spacing));
    sf::Text debugText("Toggle Debug Console", font, 18);
    debugText.setFillColor(sf::Color::White);
    debugText.setPosition(
        debugButton.getPosition().x + (debugButton.getSize().x - debugText.getGlobalBounds().width) / 2.0f,
        debugButton.getPosition().y + (debugButton.getSize().y - debugText.getGlobalBounds().height) / 2.0f
    );

    // Draw components
    window.draw(optionsPanel);
    window.draw(title);
    window.draw(resetButton);
    window.draw(resetText);
    window.draw(borderButton);
    window.draw(borderText);
    window.draw(speedText);
    window.draw(speedSlider);
    window.draw(sliderKnob);
    window.draw(debugButton);
    window.draw(debugText);

    // Cache buttons and slider for event handling
    this->resetButton = resetButton;
    this->borderButton = borderButton;
    this->debugButton = debugButton;
    this->speedSlider = speedSlider;
    this->sliderKnob = sliderKnob;
}


void UI::handleOptionsEvents(sf::RenderWindow& window, sf::Event& event, Game& game) {
    if (!showOptionsPanel) return;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    // Hover effect for buttons
    resetButton.setFillColor(resetButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(120, 120, 120) : sf::Color(100, 100, 100));
    borderButton.setFillColor(borderButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(120, 120, 120) : sf::Color(100, 100, 100));
    debugButton.setFillColor(debugButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(120, 120, 120) : sf::Color(100, 100, 100));

    // Handle button clicks
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (resetButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            game.resetSimulation();
        }
        if (borderButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            game.toggleTileBorders();
        }
        if (debugButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            std::cout << "Debug button clicked.\n";
            getDebugConsole().toggle(); 
            getDebugConsole().log("UI", "Debug Console Toggled");
        }

        if (speedSlider.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            sliderDragging = true;
        }
    }

    // Handle slider dragging
    if (sliderDragging && event.type == sf::Event::MouseMoved) {
        float sliderX = static_cast<float>(mousePos.x);
        float sliderLeft = speedSlider.getPosition().x;
        float sliderRight = speedSlider.getPosition().x + speedSlider.getSize().x;

        // Constrain knob position
        sliderX = std::clamp(sliderX, sliderLeft, sliderRight);
        sliderKnob.setPosition(sliderX - sliderKnob.getSize().x / 2, sliderKnob.getPosition().y);

        // Calculate new simulation speed
        float sliderProgress = (sliderKnob.getPosition().x - sliderLeft) / (sliderRight - sliderLeft);
        updateSliderValue(sliderProgress, game);
    }

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        sliderDragging = false;
    }
}

void UI::updateSliderValue(float newValue, Game& game) {
    currentSpeed = std::clamp(newValue, 0.0f, 3.0f);  // Keep within range
    sliderKnob.setPosition(
        speedSlider.getPosition().x + currentSpeed * (speedSlider.getSize().x - sliderKnob.getSize().x),
        sliderKnob.getPosition().y
    );
    game.setSimulationSpeed(currentSpeed);  // Update game simulation speed
    getDebugConsole().log("UI", "Simulation speed updated to: " + std::to_string(currentSpeed));
}



// Draw Graph
void UI::drawMarketGraph(sf::RenderWindow& window, const Market& market) {
    float startX = marketPanel.getGlobalBounds().left + 30.0f;
    float startY = marketPanel.getGlobalBounds().top + marketPanel.getSize().y - 150.0f;
    float graphWidth = marketPanel.getSize().x - 60.0f;
    float graphHeight = 120.0f;

    sf::RectangleShape graphBackground(sf::Vector2f(graphWidth, graphHeight));
    graphBackground.setPosition(startX, startY);
    graphBackground.setFillColor(sf::Color(25, 25, 25, 230));
    graphBackground.setOutlineThickness(1);
    graphBackground.setOutlineColor(sf::Color::White);
    window.draw(graphBackground);

    std::unordered_map<std::string, sf::Color> colors = {
        {"wood", sf::Color::Red},
        {"stone", sf::Color::Green},
        {"bush", sf::Color::Blue}
    };

    for (const auto& [resource, prices] : market.getPriceTrendMap()) {
        if (prices.empty() || colors.find(resource) == colors.end()) continue;

        sf::VertexArray line(sf::LineStrip, prices.size());
        float maxPrice = *std::max_element(prices.begin(), prices.end());
        float minPrice = *std::min_element(prices.begin(), prices.end());

        for (size_t i = 0; i < prices.size(); ++i) {
            float x = startX + (i / float(prices.size() - 1)) * graphWidth;
            float y = startY + graphHeight - ((prices[i] - minPrice) / (maxPrice - minPrice)) * graphHeight;
            line[i].position = sf::Vector2f(x, y);
            line[i].color = colors[resource];
        }
        window.draw(line);
    }
}




void UI::updateAll(const std::vector<NPCEntity>& npcs, const Market& market, const TimeManager& timeManager) {
    updateStats(npcs, timeManager);
    updateMarketPanel(market);
    updateNPCList(npcs);
}


// Render Market Panel
void UI::renderMarketPanel(sf::RenderWindow& window, const Market& market) {
    window.draw(marketPanel);  // Draw the background panel
    window.draw(advancedMarketStatsText);  // Draw title first

    drawMarketGraph(window, market);  // Graph should be drawn before text

    for (const auto& text : marketResourceTexts) {
        window.draw(text);  // Draw resource stats after the graph
    }
}




void UI::updateNPCEntityList(const std::vector<NPCEntity>& npcs) {
    npcListPanel.clearChildren();  // Clear children safely
    npcButtons.clear();            // Clear cached button references

    float buttonY = npcListPanel.getBounds().top + 20;

    for (const auto& npc : npcs) {
        UIButton* button = new UIButton();
        button->setProperties(
            npcListPanel.getBounds().left + 20,
            buttonY,
            npcListPanel.getBounds().width - 40,
            40,
            npc.getName(),
            font
        );
        button->setColors(
            sf::Color(80, 80, 150, 204),
            sf::Color(100, 100, 200, 204),
            sf::Color(60, 60, 120, 204),
            sf::Color::White
        );

        npcListPanel.addChild(button);  // Add safely
        npcButtons.emplace_back(npc.getName(), button);
        buttonY += 50;
    }
}

void UI::handleNPCEntityPanel(sf::RenderWindow& window, sf::Event& event, const std::vector<NPCEntity>& npcs) {
    if (showNPCDetail && selectedNPCIndex >= 0 && selectedNPCIndex < static_cast<int>(npcs.size())) {
        populateNPCDetails(npcs[selectedNPCIndex]); // Force update while open
    }
}

