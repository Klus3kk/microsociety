#include "UI.hpp"

UI::UI()
    : npcButton(20, 520, 100, 50, "NPC"),
      statsButton(140, 520, 100, 50, "STATS"),
      optionsButton(260, 520, 100, 50, "OPTIONS") {

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
}

void UI::updateStatus(int day, const std::string& time, int npcCount, int totalMoney) {
    std::ostringstream status;
    status << "Day: " << day << "\nTime: " << time << "\nNPCs: " << npcCount << "\nTotal Money: $" << totalMoney;
    statusText.setString(status.str());
}

void UI::handleButtonClicks(sf::RenderWindow& window, sf::Event& event) {
    if (npcButton.isClicked(window, event)) {
        tooltipPanel.setPosition(20, 100); // Slide in
        setTooltipContent("NPC Stats:\n...");
    } else if (statsButton.isClicked(window, event)) {
        tooltipPanel.setPosition(20, 100); // Slide in
        setTooltipContent("Simulation Stats:\n...");
    } else if (optionsButton.isClicked(window, event)) {
        tooltipPanel.setPosition(20, 100); // Slide in
        setTooltipContent("Options Menu:\n...");
    }
}

void UI::handleHover(sf::RenderWindow& window) {
    npcButton.handleHover(window);
    statsButton.handleHover(window);
    optionsButton.handleHover(window);
}

void UI::render(sf::RenderWindow& window) {
    // Draw status panel
    window.draw(statusPanel);
    window.draw(statusText);

    // Draw tooltip panel
    window.draw(tooltipPanel);
    window.draw(tooltipText);

    // Draw buttons
    npcButton.draw(window);
    statsButton.draw(window);
    optionsButton.draw(window);
}

void UI::setTooltipContent(const std::string& content) {
    tooltipText.setString(content);
}
