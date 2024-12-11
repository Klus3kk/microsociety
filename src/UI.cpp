
#include "UI.hpp"
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include "Game.hpp"
<<<<<<< HEAD
UI::UI() 
    : npcListPanel(npcListWidth, npcListHeight, "NPC List"), npcDetailPanel(400, 600, "NPC Details"), statsPanel(500, 400, "Simulation Stats"), marketPanel(sf::Vector2f(600, 400)) {
=======
UI::UI()
    : npcListPanel(npcListWidth, npcListHeight, "NPC List"),
      npcDetailPanel(400, 600, "NPC Details"),
      statsPanel(500, 400, ""),
      marketPanel(sf::Vector2f(600, 400)) {
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
    if (!font.loadFromFile("../assets/fonts/font.ttf")) {
        throw std::runtime_error("Failed to load font!");
    }

<<<<<<< HEAD
    marketPanel.setSize({600, 400}); // Set appropriate size
    marketPanel.setPosition(100, 100); // Adjust position as needed
    marketPanel.setFillColor(sf::Color(30, 30, 30, 150)); // Light gray with transparency
    applyShadow(marketPanel); // Optional, apply a shadow if needed
=======
    // Market Panel
    marketPanel.setSize({600, 400});
    marketPanel.setPosition(100, 100);
    marketPanel.setFillColor(UIStyles::PanelBackground);
    applyShadow(marketPanel);
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db

    // Advanced Market Stats Text
    advancedMarketStatsText.setFont(font);
    advancedMarketStatsText.setCharacterSize(14);
    advancedMarketStatsText.setFillColor(sf::Color::White);

<<<<<<< HEAD
    npcListPanel.setPosition(50, 100); // Place it on the left side
    npcListPanel.setSize(npcListWidth, npcListHeight);
    npcDetailText.setFont(font);
    npcDetailText.setCharacterSize(16);
    npcDetailText.setFillColor(sf::Color::White);
=======
    // NPC List Panel
    npcListPanel.setPosition(50, 100);
    npcListPanel.setSize(npcListWidth, npcListHeight);
    npcDetailText.setFont(font);
    npcDetailText.setCharacterSize(16);
    npcDetailText.setFillColor(UIStyles::TextColor);
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
    npcDetailText.setPosition(npcDetailPanel.getBounds().left + 20, npcDetailPanel.getBounds().top + 20);

    // Stats Text
    statsText.setFont(font);
    statsText.setCharacterSize(16);
<<<<<<< HEAD
    statsText.setFillColor(sf::Color::White);
=======
    statsText.setFillColor(UIStyles::TextColor);
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
    statsText.setPosition(statsPanel.getBounds().left + 20, statsPanel.getBounds().top + 20);

    // Money Panel
    moneyPanel.setSize({100, 50});
    moneyPanel.setPosition(20, 20);
    moneyPanel.setFillColor(sf::Color(30, 30, 30));
    applyShadow(moneyPanel);

    moneyText.setFont(font);
    moneyText.setCharacterSize(20);
<<<<<<< HEAD
    moneyText.setFillColor(sf::Color::Black);
=======
    moneyText.setFillColor(UIStyles::TextColor);
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
    moneyText.setPosition(moneyPanel.getPosition().x + 10, moneyPanel.getPosition().y + 10);

    // Status Panel
    statusPanel.setSize({200, 80});
    statusPanel.setPosition(300, 20); // Centered horizontally
    statusPanel.setFillColor(sf::Color(55, 55, 55));
    applyShadow(statusPanel);

    statusText.setFont(font);
    statusText.setCharacterSize(16);
<<<<<<< HEAD
    statusText.setFillColor(sf::Color::Black);
    statusText.setPosition(statusPanel.getPosition().x + 10, statusPanel.getPosition().y + 10);

    // Buttons (Bottom: NPC, Stats, Market, Options)
=======
    statusText.setFillColor(UIStyles::TextColor);
    statusText.setPosition(statusPanel.getPosition().x + 10, statusPanel.getPosition().y + 10);

    
    // Buttons (NPC, Stats, Market, Options)
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
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
<<<<<<< HEAD
    npcButton.setColors(sf::Color(200, 200, 200, 204), sf::Color(220, 220, 220, 204), sf::Color(160, 160, 160, 204), sf::Color(120, 120, 120));
    statsButton.setColors(sf::Color(200, 200, 200, 204), sf::Color(220, 220, 220, 204), sf::Color(160, 160, 160, 204), sf::Color(120, 120, 120));
    marketButton.setColors(sf::Color(200, 200, 200, 204), sf::Color(220, 220, 220, 204), sf::Color(160, 160, 160, 204), sf::Color(120, 120, 120));
    optionsButton.setColors(sf::Color(200, 200, 200, 204), sf::Color(220, 220, 220, 204), sf::Color(160, 160, 160, 204), sf::Color(120, 120, 120));
}


=======
    npcButton.setColors(UIStyles::ButtonNormal, UIStyles::ButtonHover, sf::Color(160, 160, 160, 204), UIStyles::TextColor);
    statsButton.setColors(UIStyles::ButtonNormal, UIStyles::ButtonHover, sf::Color(160, 160, 160, 204), UIStyles::TextColor);
    marketButton.setColors(UIStyles::ButtonNormal, UIStyles::ButtonHover, sf::Color(160, 160, 160, 204), UIStyles::TextColor);
    optionsButton.setColors(UIStyles::ButtonNormal, UIStyles::ButtonHover, sf::Color(160, 160, 160, 204), UIStyles::TextColor);
}



>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
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



<<<<<<< HEAD

=======
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
void UI::showNPCDetails(const std::string& npcDetails) {
    npcDetailPanel.setSize(400, 600);
    npcDetailPanel.setTitle("NPC Details");
    npcDetailText.setFont(font);
    npcDetailText.setCharacterSize(16);
    npcDetailText.setFillColor(sf::Color::White);
<<<<<<< HEAD
    npcDetailText.setPosition(npcDetailPanel.getBounds().left + 20, npcDetailPanel.getBounds().top + 20);
=======
    npcDetailPanel.setPosition((sf::VideoMode::getDesktopMode().width - 400) / 2, 120);
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
    npcDetailText.setString(npcDetails);
}




<<<<<<< HEAD
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


=======
void UI::handleButtonClicks(sf::RenderWindow& window, sf::Event& event, std::vector<PlayerEntity>& npcs, const TimeManager& timeManager, const Market& market) {
    if (npcButton.isClicked(window, event)) {
        showNPCList = !showNPCList; // Toggle NPC list visibility
        showNPCDetail = false;   
        std::cout << "NPC button clicked.\n";
    }

    if (marketButton.isClicked(window, event)) {
        showMarketPanel = !showMarketPanel; // Toggle market panel visibility
        if (showMarketPanel) {
            updateMarketPanel(market);
        }
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


>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
    // Handle NPC list clicks
    if (showNPCList) {
        for (size_t i = 0; i < npcButtons.size(); ++i) {
            if (npcButtons[i].second->isClicked(window, event)) {
                selectedNPCIndex = static_cast<int>(i);
                populateNPCDetails(npcs[selectedNPCIndex]); // Show NPC details
                showNPCDetail = true;
<<<<<<< HEAD
                showNPCList = false; // Hide list when details are shown
=======
                showNPCList = false;
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
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

    // Title
    statsStream << "Simulation Stats:\n\n";

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

    // Update the stats text
    statsText.setString(statsStream.str());

    // Ensure text is dynamically centered and formatted
    statsText.setPosition(statsPanel.getBounds().left + 20, statsPanel.getBounds().top + 50);
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
    // npcListPanel.setTitle("");
    npcListPanel.clearChildren();

    float startY = npcListPanel.getBounds().top + 50; // Start below the title
    float buttonSpacing = 10.0f;

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



void UI::render(sf::RenderWindow& window, const Market& market, const std::vector<PlayerEntity>& npcs) {
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
        // Draw the title last so it's above the text
        sf::Text title("Simulation Stats", font, 20);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold);
        title.setPosition(
            statsPanel.getBounds().left + (statsPanel.getBounds().width - title.getGlobalBounds().width) / 2,
            statsPanel.getBounds().top + 10
        );
        window.draw(title);
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


    // NPC List Panel
    npcListPanel.setPosition(
        (size.x - npcListPanel.getBounds().width) / 2 - 220, // Offset left
        size.y * 0.1f
    );

    // NPC Details Panel
    npcDetailPanel.setPosition(
        (size.x - npcDetailPanel.getBounds().width) / 2 + 220, // Offset right
        size.y * 0.1f
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




// Update Market Panel
void UI::updateMarketPanel(const Market& market) {
    // Clear the advancedMarketStatsText to ensure no overlap
    advancedMarketStatsText.setString("");

    std::ostringstream statsStream;

    // Start positions for the horizontal layout
    float startX = marketPanel.getPosition().x + 20;
    float startY = marketPanel.getPosition().y + 50;
    float spacingX = 200.0f; // Horizontal spacing between resource blocks

    int index = 0; // Index to track the current resource
    for (const auto& [resource, price] : market.getPrices()) {
        float posX = startX + index * spacingX;

<<<<<<< HEAD
        // Access supply and demand via Market methods
        if (market.getPrices().count(resource)) {
            supplyDemandRatio = market.getPrices().at(resource); // Replace with proper supply-demand ratio logic
        }

        statsStream << "- " << resource << ":\n";
=======
        statsStream << resource << ":\n";
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
        statsStream << "  Price: $" << price << "\n";
        statsStream << "  Buy Transactions: " << market.getBuyTransactions(resource) << "\n";
        statsStream << "  Sell Transactions: " << market.getSellTransactions(resource) << "\n";
        statsStream << "  Revenue: $" << market.getRevenue(resource) << "\n";
        statsStream << "  Expenditure: $" << market.getExpenditure(resource) << "\n";
        statsStream << "  Volatility: " << market.calculateVolatility(resource) << "\n";

        // Draw the text block for each resource
        sf::Text resourceText(statsStream.str(), font, 14);
        resourceText.setFillColor(sf::Color::White);
        resourceText.setPosition(posX, startY);

        // Add the generated text block to the panel directly
        marketResourceTexts.push_back(resourceText);

        // Clear the stream for the next resource
        statsStream.str("");
        statsStream.clear();

        index++;
    }
}

<<<<<<< HEAD
=======


>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
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
            getDebugConsole().toggle(); 
            getDebugConsole().log("UI", "Debug Console Toggled");
        }
        if (speedSlider.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            sliderDragging = true;
        }
    }

    // Handle slider dragging
    if (event.type == sf::Event::MouseMoved && sliderDragging) {
        float sliderX = static_cast<float>(mousePos.x);
        float sliderLeft = speedSlider.getPosition().x;
        float sliderRight = speedSlider.getPosition().x + speedSlider.getSize().x;

        // Constrain knob to slider bounds
        sliderX = std::clamp(sliderX, sliderLeft, sliderRight);

        // Update knob position
        sliderKnob.setPosition(sliderX - sliderKnob.getSize().x / 2, sliderKnob.getPosition().y);

        // Calculate simulation speed
        float sliderProgress = (sliderKnob.getPosition().x - sliderLeft) / (sliderRight - sliderLeft);
        currentSpeed = sliderProgress;
        game.setSimulationSpeed(sliderProgress);
    }

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        sliderDragging = false;
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
    window.draw(marketPanel); // Draw the panel background
    window.draw(advancedMarketStatsText); // Static market stats

<<<<<<< HEAD
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
=======
    // Draw each resource text block
    for (const auto& text : marketResourceTexts) {
        window.draw(text);
    }
}


>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
