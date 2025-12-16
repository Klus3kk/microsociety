#ifndef STARTUP_MENU_HPP
#define STARTUP_MENU_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>

enum class SimulationMode {
    ReinforcementLearningCPP,
    DeepQLearningTF,
    Exit
};

class StartupMenu {
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;
    std::vector<sf::RectangleShape> optionBackgrounds;
    std::vector<std::string> optionDescriptions;
    std::vector<sf::Text> descriptionTexts;
    sf::RectangleShape descriptionBackground;
    
    int selectedOption = 0;
    SimulationMode selectedMode = SimulationMode::ReinforcementLearningCPP;
    
    // Style variables
    sf::Color backgroundColor = sf::Color(30, 30, 40);
    sf::Color titleColor = sf::Color(255, 255, 255);
    sf::Color normalTextColor = sf::Color(200, 200, 200);
    sf::Color selectedTextColor = sf::Color(255, 255, 100);
    sf::Color normalBackgroundColor = sf::Color(50, 50, 60, 200);
    sf::Color selectedBackgroundColor = sf::Color(70, 70, 90, 200);
    sf::Color descriptionBgColor = sf::Color(40, 40, 50, 220);
    
    void initializeWindow();
    void initializeFont();
    void createMenuOptions();
    void handleInput();
    void render();
    void updateSelection(int newSelection);
    void updateDescriptions();

public:
    StartupMenu();
    ~StartupMenu() = default;
    
    SimulationMode run();
};

#endif 
