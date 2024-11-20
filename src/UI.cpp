#include "UI.hpp"
#include <sstream>
#include <stdexcept>
#include <iostream>

UI::UI() {
    // Load font
    if (!font.loadFromFile("../assets/fonts/font.ttf")) {
        throw std::runtime_error("Failed to load font!");
    }

    // Status Panel
    statusPanel.setSize({300, 100});
    statusPanel.setPosition(20, 20);
    statusPanel.setFillColor(sf::Color(50, 50, 50, 200));
    statusPanel.setOutlineThickness(2);
    statusPanel.setOutlineColor(sf::Color::White);

    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition(30, 30);

    // Inventory Panel
    inventoryPanel.setSize({300, 150});
    inventoryPanel.setPosition(20, 130); // Below the status panel
    inventoryPanel.setFillColor(sf::Color(50, 50, 50, 200));
    inventoryPanel.setOutlineThickness(2);
    inventoryPanel.setOutlineColor(sf::Color::White);

    inventoryText.setFont(font);
    inventoryText.setCharacterSize(16);
    inventoryText.setFillColor(sf::Color::White);
    inventoryText.setPosition(30, 140);

    // Tooltip Panel
    tooltipPanel.setSize({200, 300});
    tooltipPanel.setPosition(-220, 100); // Initially off-screen
    tooltipPanel.setFillColor(sf::Color(30, 30, 30, 200));
    tooltipPanel.setOutlineThickness(2);
    tooltipPanel.setOutlineColor(sf::Color::White);

    tooltipText.setFont(font);
    tooltipText.setCharacterSize(16);
    tooltipText.setFillColor(sf::Color::White);
    tooltipText.setPosition(tooltipPanel.getPosition().x + 10, tooltipPanel.getPosition().y + 10);

    // Buttons
    npcButton.setProperties(20, 520, 100, 50, "NPC", font);
    statsButton.setProperties(140, 520, 100, 50, "STATS", font);
    optionsButton.setProperties(260, 520, 100, 50, "OPTIONS", font);

    npcButton.setColors(sf::Color(70, 70, 70, 255), sf::Color(100, 100, 100, 255));
    statsButton.setColors(sf::Color(70, 70, 70, 255), sf::Color(100, 100, 100, 255));
    optionsButton.setColors(sf::Color(70, 70, 70, 255), sf::Color(100, 100, 100, 255));

}

void UI::updateStatus(int day, const std::string& time, int npcCount, int totalMoney, const std::unordered_map<std::string, int>& allResources) {
    std::ostringstream status;
    status << "Day: " << day << "\nTime: " << time << "\nNPCs: " << npcCount << "\nTotal Money: $" << totalMoney;
    statusText.setString(status.str());

    std::ostringstream inventoryStream;
    inventoryStream << "Resources:\n";
    for (const auto& [item, quantity] : allResources) {
        inventoryStream << "- " << item << ": " << quantity << "\n";
    }
    inventoryText.setString(inventoryStream.str());
}

void UI::handleButtonClicks(sf::RenderWindow& window, sf::Event& event) {
    if (npcButton.isClicked(window, event)) {
        tooltipPanel.setPosition(20, 100);
        setTooltipContent("NPC Stats:\nClick to view NPC details!");
    } else if (statsButton.isClicked(window, event)) {
        tooltipPanel.setPosition(20, 100);
        setTooltipContent("Simulation Stats:\nSummary info!");
    } else if (optionsButton.isClicked(window, event)) {
        tooltipPanel.setPosition(20, 100);
        setTooltipContent("Options Menu:\n- Toggle Debug\n- Exit");
    }
}

void UI::handleHover(sf::RenderWindow& window) {
    // Handle hover effects for buttons
    npcButton.handleHover(window);
    statsButton.handleHover(window);
    optionsButton.handleHover(window);
}

void UI::render(sf::RenderWindow& window) {
    // Draw UI elements
    window.draw(statusPanel);
    window.draw(statusText);

    window.draw(inventoryPanel);
    window.draw(inventoryText);

    window.draw(tooltipPanel);
    window.draw(tooltipText);

    npcButton.draw(window);
    statsButton.draw(window);
    optionsButton.draw(window);
}

void UI::setTooltipContent(const std::string& content) {
    // Update tooltip text content
    tooltipText.setString(content);
    sf::Vector2f textPos = tooltipPanel.getPosition();
    tooltipText.setPosition(textPos.x + 10, textPos.y + 10); // Adjust position
}
