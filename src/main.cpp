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
#include <csignal>
#include "debug.hpp"
/* JSON */
#include <nlohmann/json.hpp>   // Do obsługi plików JSON
/* KLASY */
#include "Tile.hpp"
#include "Game.hpp"
#include "StartupMenu.hpp"

using std::cout;
using std::endl;

void handleCrash(int signal) {
    std::cerr << "CRASH DETECTED! Saving logs before exit..." << std::endl;
    getDebugConsole().saveLogsToFile("logs/crash_log.txt");
    std::exit(signal);
}


int main() {
    std::signal(SIGSEGV, handleCrash);  // Segmentation fault
    std::signal(SIGABRT, handleCrash);  // Abort signal
    std::signal(SIGFPE, handleCrash);   // Floating point exception

    // Show startup menu first
    StartupMenu startupMenu;
    SimulationMode selectedMode = startupMenu.run();
    
    // Exit if user chose to quit
    if (selectedMode == SimulationMode::Exit) {
        getDebugConsole().saveLogsToFile("logs/simulation_log.txt");
        std::cout << "Application exited by user." << std::endl;
        return EXIT_SUCCESS;
    }
    
    // Initialize game based on selected mode
    Game game;
    
    // Configure game based on selected mode
    switch (selectedMode) {
        case SimulationMode::ReinforcementLearningCPP:
            getDebugConsole().log("Main", "Starting Reinforcement Learning C++ mode");
            game.enableReinforcementLearning(true);
            game.enableTensorFlow(false);
            break;
            
        case SimulationMode::DeepQLearningTF:
            getDebugConsole().log("Main", "Starting Deep Q-Learning TensorFlow mode");
            game.enableReinforcementLearning(true);
            game.enableTensorFlow(true);
            break;
            
        default:
            getDebugConsole().log("Main", "Unknown mode selected, defaulting to Reinforcement Learning C++");
            game.enableReinforcementLearning(true);
            game.enableTensorFlow(false);
            break;
    }
    
    // Run the game
    game.run();

    // Ensure logs are saved before exit
    getDebugConsole().saveLogsToFile("logs/simulation_log.txt");
    std::cout << "Game closed. Logs saved successfully." << std::endl;

    return EXIT_SUCCESS;
}
