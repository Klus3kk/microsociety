#include "Game.hpp"
#include <iostream>

Game::Game() {
    window.create(sf::VideoMode(800, 600), "SFML window");
    loadResources();
}

void Game::loadResources() {
    if (!font.loadFromFile("IBMPlexSans-MediumItalic.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }
    text.setFont(font);
    text.setString("Test Text");
    text.setCharacterSize(48);
    text.setPosition(100, 100);  
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void Game::update() {

}

void Game::render() {
    window.clear(sf::Color::Red);
    window.draw(text);  
    window.display();
}