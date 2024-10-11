/* STANDARDOWE BIBLIOTEKI */
#include <iostream>
#include <string>
#include <vector>
#include <memory>
/* GRAFIKA/DŹWIĘK */
#include <SFML/Graphics.hpp>  // Do grafiki 2D
#include <SFML/Audio.hpp>     // Do dźwięku
#include <SFML/System.hpp>    // Do zarządzania czasem i wątkami
#include <SFML/Window.hpp>    // Do obsługi okien i zdarzeń
/* AI */
// #include <tensorflow/core/public/session.h>      // Główna sesja TensorFlow
/* WIELOWĄTKOWOŚĆ */
#include <thread>    // Do tworzenia i zarządzania wątkami
#include <mutex>     // Do synchronizacji wątków
#include <condition_variable>  // Do bardziej złożonej synchronizacji
/* JSON */
#include <nlohmann/json.hpp>   // Do obsługi plików JSON
/* KLASY */
#include "Entity.hpp"
using std::cout;
using std::endl;

// Referencje w klasach
void createObjects() {
    sf::RenderWindow window;
    sf::Font font;
    if(!font.loadFromFile("IBMPlexSans-MediumItalic.ttf")) return EXIT_FAILURE;
    sf::Text testText("Test Text", font, 48);
}

void createWindow(sf::RenderWindow &window) {
    window.create(sf::VideoMode(800, 600), "SFML window");
}

void gameLoop(sf::RenderWindow &window) {
    createObjects();
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::Red);
        window.draw(text);
        window.display();
    }
}

int main() {
    createWindow(window);
    gameLoop(window);
    return EXIT_SUCCESS;
}
