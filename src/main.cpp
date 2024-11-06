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
// #include <tensorflow/core/public/session.h>      // Główna sesja TensorFlow C++ API do importowania modelu z Pythona
/* WIELOWĄTKOWOŚĆ */
#include <thread>    // Do tworzenia i zarządzania wątkami
#include <mutex>     // Do synchronizacji wątków
#include <condition_variable>  // Do bardziej złożonej synchronizacji
/* JSON */
#include <nlohmann/json.hpp>   // Do obsługi plików JSON
/* KLASY */
#include "Tile.hpp"
#include "Game.hpp"

using std::cout;
using std::endl;

// Referencje w klasach

int main() {
    Game game;
    game.run();
    return EXIT_SUCCESS;
}
