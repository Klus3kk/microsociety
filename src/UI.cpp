#include "UI.hpp"
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <iostream>

UI::UI() {
    if (!font.loadFromFile("../assets/fonts/font.ttf")) {
        throw std::runtime_error("Failed to load font!");
    }

    // Status Panel
    statusPanel.setSize({300, 100});
    statusPanel.setPosition(20, 20);
    statusPanel.setFillColor(sf::Color(30, 30, 30, 230));
    applyShadow(statusPanel);

    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition(30, 30);

    // Inventory Panel
    inventoryPanel.setSize({300, 150});
    inventoryPanel.setPosition(20, 130);
    inventoryPanel.setFillColor(sf::Color(30, 30, 30, 230));
    applyShadow(inventoryPanel);

    inventoryText.setFont(font);
    inventoryText.setCharacterSize(16);
    inventoryText.setFillColor(sf::Color::White);
    inventoryText.setPosition(30, 140);

    // NPC List Panel
    npcListPanel.setSize({300, 300});
    npcListPanel.setPosition(20, 300);
    npcListPanel.setFillColor(sf::Color(30, 30, 30, 230));
    applyShadow(npcListPanel);

    // NPC Details Panel
    npcDetailPanel.setSize({400, 300});
    npcDetailPanel.setPosition(340, 300);
    npcDetailPanel.setFillColor(sf::Color(30, 30, 30, 230));
    applyShadow(npcDetailPanel);

    npcDetailText.setFont(font);
    npcDetailText.setCharacterSize(16);
    npcDetailText.setFillColor(sf::Color::White);
    npcDetailText.setPosition(350, 310);

    // Market Panel
    marketPanel.setSize({300, 200});
    marketPanel.setPosition(800, 20);
    marketPanel.setFillColor(sf::Color(30, 30, 30, 230));
    applyShadow(marketPanel);

    marketText.setFont(font);
    marketText.setCharacterSize(16);
    marketText.setFillColor(sf::Color::White);
    marketText.setPosition(810, 30);

    // Tooltip Panel
    tooltipPanel.setSize({250, 100});
    tooltipPanel.setFillColor(sf::Color(50, 50, 50, 200));
    tooltipPanel.setOutlineThickness(2);
    tooltipPanel.setOutlineColor(sf::Color::White);

    tooltipText.setFont(font);
    tooltipText.setCharacterSize(14);
    tooltipText.setFillColor(sf::Color::White);

    // Buttons
    statsButton.setProperties(20, 620, 100, 50, "STATS", font);
    optionsButton.setProperties(140, 620, 100, 50, "OPTIONS", font);

    // Updated to pass the fourth "border" argument
    statsButton.setColors(
        sf::Color(70, 70, 150), 
        sf::Color(100, 100, 200), 
        sf::Color(50, 50, 120), 
        sf::Color::White // Border color
    );
    optionsButton.setColors(
        sf::Color(70, 70, 150), 
        sf::Color(100, 100, 200), 
        sf::Color(50, 50, 120), 
        sf::Color::White // Border color
    );
}

void UI::applyShadow(sf::RectangleShape& shape, float offset) {
    shape.setOutlineThickness(offset);
    shape.setOutlineColor(sf::Color(0, 0, 0, 100));
}

void UI::updateStatus(int day, const std::string& time, int npcCount, int totalMoney, const std::unordered_map<std::string, int>& allResources) {
    std::ostringstream status;
    status << "Day: " << day << "\nTime: " << time << "\nNPCs: " << npcCount << "\nTotal Money: $" << totalMoney;
    statusText.setString(status.str());

    std::ostringstream inventoryStream;
    inventoryStream << "Inventory:\n";
    for (const auto& [item, quantity] : allResources) {
        inventoryStream << "- " << item << ": " << quantity << "\n";
    }
    inventoryText.setString(inventoryStream.str());
}

void UI::updateNPCList(const std::vector<std::string>& npcNames) {
    npcButtons.clear();
    float yOffset = 310;

    for (size_t i = 0; i < npcNames.size(); ++i) {
        UIButton npcButton;
        npcButton.setProperties(30, yOffset, 260, 40, npcNames[i], font);
        npcButton.setColors(
            sf::Color(80, 80, 150), 
            sf::Color(100, 100, 200), 
            sf::Color(60, 60, 120), 
            sf::Color::White // Border color
        );
        npcButtons.emplace_back(npcNames[i], npcButton);
        yOffset += 50;
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

    updateTooltipPosition(window);
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

    window.draw(marketPanel);
    window.draw(marketText);

    window.draw(tooltipPanel);
    statsButton.draw(window);
    optionsButton.draw(window);
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
           npcListPanel.getGlobalBounds().contains(mouseFloatPos) ||
           npcDetailPanel.getGlobalBounds().contains(mouseFloatPos) ||
           statsButton.isMouseOver(window) ||
           optionsButton.isMouseOver(window);
}
