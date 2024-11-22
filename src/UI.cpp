#include "UI.hpp"
#include <sstream>
#include <stdexcept>
#include <iostream>

UI::UI() {
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

    // NPC List Panel
    npcListPanel.setSize({300, 300});
    npcListPanel.setPosition(20, 300); // Below inventory
    npcListPanel.setFillColor(sf::Color(50, 50, 50, 200));
    npcListPanel.setOutlineThickness(2);
    npcListPanel.setOutlineColor(sf::Color::White);

    // NPC Details Panel
    npcDetailPanel.setSize({400, 300});
    npcDetailPanel.setPosition(340, 300); // To the right of the NPC list
    npcDetailPanel.setFillColor(sf::Color(50, 50, 50, 200));
    npcDetailPanel.setOutlineThickness(2);
    npcDetailPanel.setOutlineColor(sf::Color::White);

    npcDetailText.setFont(font);
    npcDetailText.setCharacterSize(16);
    npcDetailText.setFillColor(sf::Color::White);
    npcDetailText.setPosition(350, 310);

    // Buttons
    statsButton.setProperties(20, 620, 100, 50, "STATS", font);
    optionsButton.setProperties(140, 620, 100, 50, "OPTIONS", font);

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

void UI::updateNPCList(const std::vector<std::string>& npcNames) {
    npcButtons.clear(); // Clear old buttons
    float yOffset = 310; // Start position for the first button

    for (size_t i = 0; i < npcNames.size(); ++i) {
        UIButton npcButton;
        npcButton.setProperties(30, yOffset, 260, 40, npcNames[i], font);
        npcButton.setColors(sf::Color(70, 70, 70, 255), sf::Color(100, 100, 100, 255));
        npcButtons.emplace_back(npcNames[i], npcButton);
        yOffset += 50; // Space between buttons
    }
}

void UI::showNPCDetails(const std::string& npcDetails) {
    npcDetailText.setString(npcDetails);
}

void UI::handleButtonClicks(sf::RenderWindow& window, sf::Event& event) {
    for (size_t i = 0; i < npcButtons.size(); ++i) {
        if (npcButtons[i].second.isClicked(window, event)) {
            selectedNPCIndex = static_cast<int>(i);
            showNPCDetails("NPC Details:\nName: " + npcButtons[i].first + "\nHealth: 100\nEnergy: 50\nHunger: 30");
        }
    }

    if (statsButton.isClicked(window, event)) {
        tooltipPanel.setPosition(20, 100);
        setTooltipContent("Simulation Stats:\nSummary info!");
    } else if (optionsButton.isClicked(window, event)) {
        tooltipPanel.setPosition(20, 100);
        setTooltipContent("Options Menu:\n- Toggle Debug\n- Exit");
    }
}

void UI::handleHover(sf::RenderWindow& window) {
    for (auto& [name, button] : npcButtons) {
        button.handleHover(window);
    }
    statsButton.handleHover(window);
    optionsButton.handleHover(window);
}

void UI::render(sf::RenderWindow& window) {
    window.draw(statusPanel);
    window.draw(statusText);

    window.draw(inventoryPanel);
    window.draw(inventoryText);

    window.draw(npcListPanel);
    for (auto& [name, button] : npcButtons) {
        button.draw(window);
    }

    if (selectedNPCIndex != -1) {
        window.draw(npcDetailPanel);
        window.draw(npcDetailText);
    }

    statsButton.draw(window);
    optionsButton.draw(window);
}

void UI::setTooltipContent(const std::string& content) {
    tooltipText.setString(content);
}

bool UI::isMouseOver(sf::RenderWindow& window) const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mouseFloatPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Check if the mouse is over any UI element
    return statusPanel.getGlobalBounds().contains(mouseFloatPos) ||
           inventoryPanel.getGlobalBounds().contains(mouseFloatPos) ||
           npcListPanel.getGlobalBounds().contains(mouseFloatPos) ||
           npcDetailPanel.getGlobalBounds().contains(mouseFloatPos) ||
           statsButton.isMouseOver(window) ||
           optionsButton.isMouseOver(window);
}