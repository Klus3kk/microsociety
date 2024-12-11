#include "UI.hpp"
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include "Game.hpp"
UI::UI() 
    : npcListPanel(npcListWidth, npcListHeight, "NPC List"), npcDetailPanel(400, 600, "NPC Details"), statsPanel(500, 400, "Simulation Stats"), marketPanel(sf::Vector2f(600, 400)) {
    if (!font.loadFromFile("../assets/fonts/font.ttf")) {
        throw std::runtime_error("Failed to load font!");
    }

    marketPanel.setSize({600, 400}); // Set appropriate size
    marketPanel.setPosition(100, 100); // Adjust position as needed
    marketPanel.setFillColor(sf::Color(30, 30, 30, 150)); // Light gray with transparency
    applyShadow(marketPanel); // Optional, apply a shadow if needed

    advancedMarketStatsText.setFont(font);
    advancedMarketStatsText.setCharacterSize(14);
    advancedMarketStatsText.setFillColor(sf::Color::White);

    npcListPanel.setPosition(50, 100); // Place it on the left side
    npcListPanel.setSize(npcListWidth, npcListHeight);
    npcDetailText.setFont(font);
    npcDetailText.setCharacterSize(16);
    npcDetailText.setFillColor(sf::Color::White);
    npcDetailText.setPosition(npcDetailPanel.getBounds().left + 20, npcDetailPanel.getBounds().top + 20);

    // Stats Text
    statsText.setFont(font);
    statsText.setCharacterSize(16);
    statsText.setFillColor(sf::Color::White);
    statsText.setPosition(statsPanel.getBounds().left + 20, statsPanel.getBounds().top + 20);

    // Money Panel (Top Left)
    moneyPanel.setSize({100, 50});
    moneyPanel.setPosition(20, 20);
    moneyPanel.setFillColor(sf::Color(30, 30, 30));
    applyShadow(moneyPanel);

    moneyText.setFont(font);
    moneyText.setCharacterSize(20);
    moneyText.setFillColor(sf::Color::Black);
    moneyText.setPosition(moneyPanel.getPosition().x + 10, moneyPanel.getPosition().y + 10);

    // Status Panel (Top Center)
    statusPanel.setSize({200, 80});
    statusPanel.setPosition(300, 20); // Centered horizontally
    statusPanel.setFillColor(sf::Color(55, 55, 55));
    applyShadow(statusPanel);

    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::Black);
    statusText.setPosition(statusPanel.getPosition().x + 10, statusPanel.getPosition().y + 10);

    // Buttons (Bottom: NPC, Stats, Market, Options)
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
    npcButton.setColors(sf::Color(200, 200, 200, 204), sf::Color(220, 220, 220, 204), sf::Color(160, 160, 160, 204), sf::Color(120, 120, 120));
    statsButton.setColors(sf::Color(200, 200, 200, 204), sf::Color(220, 220, 220, 204), sf::Color(160, 160, 160, 204), sf::Color(120, 120, 120));
    marketButton.setColors(sf::Color(200, 200, 200, 204), sf::Color(220, 220, 220, 204), sf::Color(160, 160, 160, 204), sf::Color(120, 120, 120));
    optionsButton.setColors(sf::Color(200, 200, 200, 204), sf::Color(220, 220, 220, 204), sf::Color(160, 160, 160, 204), sf::Color(120, 120, 120));
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
           << "Iteration: " << iteration;

    statusText.setString(status.str());
}


void UI::updateNPCList(const std::vector<PlayerEntity>& npcs) {
    npcButtons.clear();
    npcListPanel.clearChildren(); // Ensure old buttons are removed

    float buttonY = npcListPanel.getBounds().top + 20;

    for (size_t i = 0; i < npcs.size(); ++i) {
        UIButton* button = new UIButton();
        button->setProperties(
            npcListPanel.getBounds().left + 20,
            buttonY,
            npcListPanel.getBounds().width - 40,
            40,
            npcs[i].getName(),
            font
        );
        button->setColors(
            sf::Color(80, 80, 150, 204),
            sf::Color(100, 100, 200, 204),
            sf::Color(60, 60, 120, 204),
            sf::Color::White
        );

        npcListPanel.addChild(button); // Link button to panel
        npcButtons.emplace_back(npcs[i].getName(), button);
        buttonY += 50;
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



void UI::populateNPCDetails(const PlayerEntity& npc) {
    // Update the text content for NPC details
    std::ostringstream details;
    details << "Name: " << npc.getName() << "\n"
            << "Health: " << npc.getHealth() << " / 100\n"
            << "Hunger: " << npc.getHunger() << "\n"
            << "Energy: " << static_cast<int>(npc.getEnergyPercentage() * 100) << "%\n"
            << "Speed: " << npc.getBaseSpeed() << "\n"
            << "Strength: " << npc.getStrength() << "\n"
            << "Money: $" << npc.getMoney() << "\n"
            << "Inventory:\n";

    for (const auto& [item, quantity] : npc.getInventory()) {
        details << "  - " << item << ": " << quantity << "\n";
    }

    // Update the detail text for the panel
    npcDetailText.setString(details.str());
}




void UI::showNPCDetails(const std::string& npcDetails) {
    npcDetailPanel.setPosition(100, 100); // Example position, adjust as needed
    npcDetailPanel.setSize(400, 600);
    npcDetailPanel.setTitle("NPC Details");
    npcDetailText.setFont(font);
    npcDetailText.setCharacterSize(16);
    npcDetailText.setFillColor(sf::Color::White);
    npcDetailText.setPosition(npcDetailPanel.getBounds().left + 20, npcDetailPanel.getBounds().top + 20);
    npcDetailText.setString(npcDetails);
}




void UI::handleButtonClicks(sf::RenderWindow& window, sf::Event& event, std::vector<PlayerEntity>& npcs, const TimeManager& timeManager) {
    if (npcButton.isClicked(window, event)) {
        showNPCList = !showNPCList; // Toggle NPC list visibility
        showNPCDetail = false;     // Hide details when list is toggled
        std::cout << "NPC button clicked.\n";
    }

    if (statsButton.isClicked(window, event)) {
        showStatsPanel = !showStatsPanel; // Toggle visibility
        if (showStatsPanel) {
            updateStats(npcs, timeManager);
        }
    }

    // if (statsButton.isClicked(window, event)) {
    //     std::cout << "Stats button clicked.\n";
    // }

    // if (marketButton.isClicked(window, event)) {
    //     std::cout << "Market button clicked.\n";
    // }

    if (optionsButton.isClicked(window, event)) {
        showOptionsPanel = !showOptionsPanel; // Toggle visibility
        std::cout << "Options button clicked. Panel visibility: " << showOptionsPanel << "\n";
    }


    // Handle NPC list clicks
    if (showNPCList) {
        for (size_t i = 0; i < npcButtons.size(); ++i) {
            if (npcButtons[i].second->isClicked(window, event)) {
                selectedNPCIndex = static_cast<int>(i);
                populateNPCDetails(npcs[selectedNPCIndex]);
                showNPCDetail = true;
                showNPCList = false; // Hide list when details are shown
                std::cout << "NPC detail shown for: " << npcButtons[i].first << "\n";
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

void UI::updateStats(const std::vector<PlayerEntity>& npcs, const TimeManager& timeManager) {
    std::ostringstream statsStream;

    // Use values from TimeManager
    statsStream << "Simulation Stats:\n\n";
    statsStream << "Day: " << timeManager.getCurrentDay() << "\n";
    statsStream << "Time: " << timeManager.getFormattedTime() << "\n";
    statsStream << "Iteration: " << timeManager.getSocietyIteration() << "\n\n";

    // NPC Stats
    statsStream << "NPC Stats:\n";
    statsStream << "  Total NPCs: " << npcs.size() << "\n";

    float totalHealth = 0, totalEnergy = 0, totalHunger = 0;
    for (const auto& npc : npcs) {
        totalHealth += npc.getHealth();
        totalEnergy += npc.getEnergy();
        totalHunger += npc.getHunger();
    }
    statsStream << "  Avg. Health: " << (npcs.empty() ? 0 : totalHealth / npcs.size()) << "\n";
    statsStream << "  Avg. Energy: " << (npcs.empty() ? 0 : totalEnergy / npcs.size()) << "\n";
    statsStream << "  Avg. Hunger: " << (npcs.empty() ? 0 : totalHunger / npcs.size()) << "\n\n";

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

    statsText.setString(statsStream.str());
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


void UI::populateNPCList(const std::vector<PlayerEntity>& npcs) {
    npcListPanel.setSize(400, 600);
    npcListPanel.setTitle("NPC List");
    updateNPCList(npcs);
}

void UI::handleNPCPanel(sf::RenderWindow& window, sf::Event& event, const std::vector<PlayerEntity>& npcs) {
    npcListPanel.handleEvent(window, event);

    // Handle scrolling
    if (event.type == sf::Event::MouseWheelScrolled && showNPCList) {
        float scrollAmount = event.mouseWheelScroll.delta * scrollSpeed;
        npcListScrollOffset += scrollAmount;

        // Constrain scrolling
        float minOffset = 0;
        float maxOffset = std::max(0.0f, static_cast<float>(npcButtons.size() * 50 - npcListPanel.getBounds().height));
        npcListScrollOffset = std::clamp(npcListScrollOffset, minOffset, maxOffset);

        // Update button positions based on scroll offset
        float buttonY = -npcListScrollOffset;
        for (auto& [_, button] : npcButtons) {
            button->setProperties(button->getPosition().x, buttonY, button->getSize().x, button->getSize().y, button->getText(), font);
            buttonY += 50;
        }
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



void UI::render(sf::RenderWindow& window, const Market& market) {
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

    if (showNPCDetail) {
        npcDetailPanel.render(window);
        window.draw(npcDetailText);
    }

    if (showStatsPanel) {
        statsPanel.render(window);
        window.draw(statsText);
    }

    // Add the missing market panel rendering logic
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




// Update Market Panel
void UI::updateMarketPanel(const Market& market) {
    std::ostringstream statsStream;
    statsStream << "Advanced Market Stats:\n\n";

    statsStream << "Total Trades: " << market.getTotalTrades() << "\n";

    const auto& resourceStats = market.getResourceStats();
    for (const auto& [resource, price] : resourceStats) {
        float volatility = market.calculateVolatility(resource);
        float supplyDemandRatio = 0.0f;

        // Access supply and demand via Market methods
        if (market.getPrices().count(resource)) {
            supplyDemandRatio = market.getPrices().at(resource); // Replace with proper supply-demand ratio logic
        }

        statsStream << "- " << resource << ":\n";
        statsStream << "  Price: $" << price << "\n";
        statsStream << "  Buy Transactions: " << market.getBuyTransactions(resource) << "\n";
        statsStream << "  Sell Transactions: " << market.getSellTransactions(resource) << "\n";
        statsStream << "  Revenue: $" << market.getRevenue(resource) << "\n";
        statsStream << "  Expenditure: $" << market.getExpenditure(resource) << "\n";
        statsStream << "  Volatility: " << volatility << "\n";
        statsStream << "  Supply-to-Demand Ratio: " << supplyDemandRatio << "\n";
    }

    advancedMarketStatsText.setString(statsStream.str());
}

void UI::renderOptionsPanel(sf::RenderWindow& window) {
    if (!showOptionsPanel) return;

    // Background
    sf::RectangleShape optionsPanel;
    optionsPanel.setSize({600, 400});
    optionsPanel.setPosition(150, 150);
    optionsPanel.setFillColor(sf::Color(50, 50, 50, 200));

    // Title
    sf::Text title("Options", font, 24);
    title.setFillColor(sf::Color::White);
    title.setPosition(optionsPanel.getPosition().x + 20, optionsPanel.getPosition().y + 20);

    // Option: Reset Simulation
    sf::Text resetText("Reset Simulation", font, 18);
    resetText.setFillColor(sf::Color::White);
    resetText.setPosition(optionsPanel.getPosition().x + 20, optionsPanel.getPosition().y + 80);

    sf::RectangleShape resetButton({150, 30});
    resetButton.setFillColor(sf::Color(100, 100, 100));
    resetButton.setPosition(resetText.getPosition().x + 200, resetText.getPosition().y);

    // Option: Toggle Tile Borders
    sf::Text borderText("Toggle Tile Borders", font, 18);
    borderText.setFillColor(sf::Color::White);
    borderText.setPosition(optionsPanel.getPosition().x + 20, optionsPanel.getPosition().y + 130);

    sf::RectangleShape borderButton({150, 30});
    borderButton.setFillColor(sf::Color(100, 100, 100));
    borderButton.setPosition(borderText.getPosition().x + 200, borderText.getPosition().y);

    // Option: Simulation Speed
    sf::Text speedText("Simulation Speed", font, 18);
    speedText.setFillColor(sf::Color::White);
    speedText.setPosition(optionsPanel.getPosition().x + 20, optionsPanel.getPosition().y + 180);

    sf::RectangleShape speedSlider({200, 10});
    speedSlider.setFillColor(sf::Color(150, 150, 150));
    speedSlider.setPosition(speedText.getPosition().x + 200, speedText.getPosition().y + 10);

    // Draw components
    window.draw(optionsPanel);
    window.draw(title);
    window.draw(resetText);
    window.draw(resetButton);
    window.draw(borderText);
    window.draw(borderButton);
    window.draw(speedText);
    window.draw(speedSlider);
}

void UI::handleOptionsEvents(sf::RenderWindow& window, sf::Event& event, Game& game) {
    if (!showOptionsPanel) return;

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Reset Simulation
        if (resetButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            game.resetSimulation();
        }

        // Toggle Tile Borders
        if (borderButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            game.toggleTileBorders();
        }

        // Simulation Speed (Slider Logic)
        if (speedSlider.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            float sliderPosition = static_cast<float>(mousePos.x - speedSlider.getPosition().x);
            game.setSimulationSpeed(sliderPosition / speedSlider.getSize().x);
        }
    }
}



// Draw Graph
void UI::drawMarketGraph(sf::RenderWindow& window, const Market& market) {
    float startX = marketPanel.getGlobalBounds().left + 20.0f;
    float startY = marketPanel.getGlobalBounds().top + 50.0f;
    float graphWidth = marketPanel.getGlobalBounds().width - 40.0f;
    float graphHeight = 150.0f;

    // Background for the graph
    sf::RectangleShape graphBackground(sf::Vector2f(graphWidth, graphHeight));
    graphBackground.setPosition(startX, startY);
    graphBackground.setFillColor(sf::Color(15, 15, 15, 220)); // Modern black background
    window.draw(graphBackground);

    // Find the maximum price for scaling
    float maxPrice = 1.0f;
    for (const auto& [resource, prices] : market.getPriceTrendMap()) {
        if (!prices.empty()) {
            maxPrice = std::max(maxPrice, *std::max_element(prices.begin(), prices.end()));
        }
    }

    // Draw price trends for each item
    int colorIndex = 0;
    std::vector<sf::Color> lineColors = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};

    for (const auto& [resource, prices] : market.getPriceTrendMap()) {
        if (prices.empty()) continue;

        sf::VertexArray line(sf::LineStrip, prices.size());
        for (size_t i = 0; i < prices.size(); ++i) {
            float x = startX + (static_cast<float>(i) / (prices.size() - 1)) * graphWidth;
            float y = startY + graphHeight - (prices[i] / maxPrice) * graphHeight;
            line[i].position = sf::Vector2f(x, y);
            line[i].color = lineColors[colorIndex % lineColors.size()];
        }
        window.draw(line);

        sf::Text label(resource, font, 12);
        label.setPosition(startX, startY + graphHeight + 5 + (15 * colorIndex));
        label.setFillColor(lineColors[colorIndex % lineColors.size()]);
        window.draw(label);

        colorIndex++;
    }
}



// Render Market Panel
void UI::renderMarketPanel(sf::RenderWindow& window, const Market& market) {
    window.draw(marketPanel); // Correct rendering for a RectangleShape
    drawMarketGraph(window, market);

    sf::Text statsText(advancedMarketStatsText);
    statsText.setPosition(marketPanel.getGlobalBounds().left + 20, marketPanel.getGlobalBounds().top + 220);
    window.draw(statsText);
}


// Handle Market Button
void UI::handleMarketButton(sf::RenderWindow& window, sf::Event& event, const Market& market) {
    if (marketButton.isClicked(window, event)) {
        showMarketPanel = !showMarketPanel; // Toggle visibility
        if (showMarketPanel) {
            updateMarketPanel(market);
        }
    }
}
