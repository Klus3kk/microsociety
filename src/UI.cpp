#include "UI.hpp"
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <iostream>

UI::UI() 
    : npcListPanel(400, 600, "NPC List"), npcDetailPanel(400, 600, "NPC Details") {
    if (!font.loadFromFile("../assets/fonts/font.ttf")) {
        throw std::runtime_error("Failed to load font!");
    }

    npcDetailText.setFont(font);
    npcDetailText.setCharacterSize(16);
    npcDetailText.setFillColor(sf::Color::White);
    npcDetailText.setPosition(npcDetailPanel.getBounds().left + 20, npcDetailPanel.getBounds().top + 20);

    // Money Panel (Top Left)
    moneyPanel.setSize({100, 50});
    moneyPanel.setPosition(20, 20);
    moneyPanel.setFillColor(sf::Color(230, 230, 230));
    applyShadow(moneyPanel);

    moneyText.setFont(font);
    moneyText.setCharacterSize(20);
    moneyText.setFillColor(sf::Color::Black);
    moneyText.setPosition(moneyPanel.getPosition().x + 10, moneyPanel.getPosition().y + 10);

    // Status Panel (Top Center)
    statusPanel.setSize({200, 80});
    statusPanel.setPosition(300, 20); // Centered horizontally
    statusPanel.setFillColor(sf::Color(255, 255, 255));
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
    for (size_t i = 0; i < npcs.size(); ++i) {
        UIButton button;
        button.setProperties(20, 20 + 50 * i, 360, 40, npcs[i].getName(), font);
        button.setColors(
            sf::Color(80, 80, 150), 
            sf::Color(100, 100, 200), 
            sf::Color(60, 60, 120), 
            sf::Color::White
        );
        npcButtons.emplace_back(npcs[i].getName(), button);
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




void UI::handleButtonClicks(sf::RenderWindow& window, sf::Event& event, std::vector<PlayerEntity>& npcs) {
    if (npcButton.isClicked(window, event)) {
        showNPCList = !showNPCList; // Toggle NPC list visibility
        showNPCDetail = false;     // Hide details when list is toggled
        std::cout << "NPC button clicked.\n";
    }

    if (statsButton.isClicked(window, event)) {
        std::cout << "Stats button clicked.\n";
    }

    if (marketButton.isClicked(window, event)) {
        std::cout << "Market button clicked.\n";
    }

    if (optionsButton.isClicked(window, event)) {
        std::cout << "Options button clicked.\n";
    }

    // Handle NPC list clicks
    if (showNPCList) {
        for (size_t i = 0; i < npcButtons.size(); ++i) {
            if (npcButtons[i].second.isClicked(window, event)) {
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




void UI::handleHover(sf::RenderWindow& window) {
    npcButton.handleHover(window);
    statsButton.handleHover(window);
    marketButton.handleHover(window);
    optionsButton.handleHover(window);

    if (showNPCList) {
        for (auto& [_, button] : npcButtons) {
            button.handleHover(window);
        }
    }   
}

void UI::drawPriceTrends(sf::RenderWindow& window, const Market& market) {
    float startX = marketPanel.getPosition().x + 20.0f;
    float startY = marketPanel.getPosition().y + 80.0f;
    float graphWidth = 260.0f;
    float graphHeight = 100.0f;

    sf::RectangleShape graphBackground(sf::Vector2f(graphWidth, graphHeight));
    graphBackground.setPosition(startX, startY);
    graphBackground.setFillColor(sf::Color(20, 20, 20, 180));
    window.draw(graphBackground);

    float maxPrice = 1.0f;
    for (const auto& [item, prices] : market.getPriceTrendMap()) {
        if (!prices.empty()) {
            maxPrice = *std::max_element(prices.begin(), prices.end());
        }
    }
    if (maxPrice == 0.0f) maxPrice = 1.0f; // Prevent division by zero

    int colorIndex = 0;
    std::vector<sf::Color> lineColors = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};

    for (const auto& [item, prices] : market.getPriceTrendMap()) {
        if (prices.empty()) continue;

        sf::VertexArray line(sf::LineStrip, prices.size());
        for (size_t i = 0; i < prices.size(); ++i) {
            float x = startX + (static_cast<float>(i) / (prices.size() - 1)) * graphWidth;
            float y = startY + graphHeight - (prices[i] / maxPrice) * graphHeight;
            line[i].position = sf::Vector2f(x, y);
            line[i].color = lineColors[colorIndex % lineColors.size()];
        }
        window.draw(line);

        // Add labels
        sf::Text label(item, font, 12);
        label.setPosition(startX, startY + graphHeight + 5 + (15 * colorIndex));
        label.setFillColor(lineColors[colorIndex % lineColors.size()]);
        window.draw(label);

        colorIndex++;
    }
}

void UI::populateNPCList(const std::vector<PlayerEntity>& npcs) {
    npcListPanel.setSize(400, 600);
    npcListPanel.setTitle("NPC List");
    updateNPCList(npcs);
}

void UI::handleNPCPanel(sf::RenderWindow& window, sf::Event& event) {
    npcDetailPanel.handleEvent(window, event);
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
            npcButtons[i].second.draw(window);
            std::cout << "Drawing NPC button: " << npcButtons[i].first << "\n";
        }
    }

    if (showNPCDetail) {
        npcDetailPanel.render(window);
        window.draw(npcDetailText);
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



