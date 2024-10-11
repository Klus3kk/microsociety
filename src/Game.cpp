#include "Game.hpp"
#include <iostream>

Game::Game() {
    window.create(sf::VideoMode(800, 600), "SFML window");
    loadResources();
}

void Game::loadResources() [
    if (!font.loadFromFile("IBMPlexSans-MediumItalic.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }
    text.setFont(font);
    text.setString("Test Text");
    text.setCharacterSize(48);
    text.setPosition(100, 100);  // Example positioning
]

