#include "StartupMenu.hpp"
#include "debug.hpp"
#include <iostream>

StartupMenu::StartupMenu() {
    initializeWindow();
    initializeFont();
    createMenuOptions();
}

void StartupMenu::initializeWindow() {
    // Create window with a smaller size than the main game
    window.create(sf::VideoMode(800, 600), "MicroSociety - Startup Menu", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
}

void StartupMenu::initializeFont() {
    // Load font
    if (!font.loadFromFile("../assets/fonts/font.ttf")) {
        getDebugConsole().log("StartupMenu", "Failed to load font!", LogLevel::Error);
        std::cerr << "Failed to load font!" << std::endl;
    }
}

void StartupMenu::createMenuOptions() {
    // Set up title
    titleText.setFont(font);
    titleText.setString("MicroSociety Simulation");
    titleText.setCharacterSize(40);
    titleText.setFillColor(titleColor);
    titleText.setStyle(sf::Text::Bold);
    
    // Center the title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition(
        (window.getSize().x - titleBounds.width) / 2.0f, 
        50.0f
    );
    
    // Create menu options
    std::vector<std::string> options = {
        "1. Single Player Mode",
        "2. Reinforcement Learning C++",
        "3. Deep Q-Learning TensorFlow",
        "4. Exit"
    };
    
    optionDescriptions = {
        "Play as a character in the simulation and interact with the environment and NPCs.",
        "Run simulation with NPCs using classic reinforcement learning algorithms.",
        "Run simulation with NPCs using TensorFlow Deep Q-Learning neural networks.",
        "Exit the application."
    };
    
    float startY = 180.0f;
    float optionHeight = 50.0f;
    float padding = 10.0f;
    float spaceBetween = 20.0f;
    
    for (size_t i = 0; i < options.size(); i++) {
        // Create text
        sf::Text optionText;
        optionText.setFont(font);
        optionText.setString(options[i]);
        optionText.setCharacterSize(24);
        optionText.setFillColor(i == selectedOption ? selectedTextColor : normalTextColor);
        
        // Position text
        sf::FloatRect textBounds = optionText.getLocalBounds();
        float textX = (window.getSize().x - textBounds.width) / 2.0f;
        float textY = startY + i * (optionHeight + spaceBetween) + padding;
        optionText.setPosition(textX, textY);
        
        // Create background
        sf::RectangleShape background(sf::Vector2f(window.getSize().x - 200.0f, optionHeight));
        background.setFillColor(i == selectedOption ? selectedBackgroundColor : normalBackgroundColor);
        background.setPosition(100.0f, startY + i * (optionHeight + spaceBetween));
        
        menuOptions.push_back(optionText);
        optionBackgrounds.push_back(background);
    }
    
    // Create description area
    descriptionBackground.setSize(sf::Vector2f(window.getSize().x - 100.0f, 100.0f));
    descriptionBackground.setFillColor(descriptionBgColor);
    descriptionBackground.setPosition(50.0f, 450.0f);
    
    // Create description texts
    for (const auto& desc : optionDescriptions) {
        sf::Text descText;
        descText.setFont(font);
        descText.setString(desc);
        descText.setCharacterSize(18);
        descText.setFillColor(normalTextColor);
        
        // Position will be updated in updateDescriptions
        descriptionTexts.push_back(descText);
    }
    
    updateDescriptions();
}

void StartupMenu::updateSelection(int newSelection) {
    // Bounds check
    if (newSelection < 0 || newSelection >= static_cast<int>(menuOptions.size())) {
        return;
    }
    
    // Update visual state
    menuOptions[selectedOption].setFillColor(normalTextColor);
    optionBackgrounds[selectedOption].setFillColor(normalBackgroundColor);
    
    selectedOption = newSelection;
    
    menuOptions[selectedOption].setFillColor(selectedTextColor);
    optionBackgrounds[selectedOption].setFillColor(selectedBackgroundColor);
    
    // Update the selected mode
    switch (selectedOption) {
        case 0: selectedMode = SimulationMode::SinglePlayer; break;
        case 1: selectedMode = SimulationMode::ReinforcementLearningCPP; break;
        case 2: selectedMode = SimulationMode::DeepQLearningTF; break;
        case 3: selectedMode = SimulationMode::Exit; break;
        default: selectedMode = SimulationMode::ReinforcementLearningCPP; break;
    }
    
    updateDescriptions();
}

void StartupMenu::updateDescriptions() {
    // Only show the description for the selected option
    for (size_t i = 0; i < descriptionTexts.size(); i++) {
        if (i == static_cast<size_t>(selectedOption)) {
            descriptionTexts[i].setString(optionDescriptions[i]);
            
            // Center the text in the description area
            sf::FloatRect textBounds = descriptionTexts[i].getLocalBounds();
            float textX = descriptionBackground.getPosition().x + 
                         (descriptionBackground.getSize().x - textBounds.width) / 2.0f;
            float textY = descriptionBackground.getPosition().y + 
                         (descriptionBackground.getSize().y - textBounds.height) / 2.0f - 10.0f;
            
            descriptionTexts[i].setPosition(textX, textY);
        }
    }
}

void StartupMenu::handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            selectedMode = SimulationMode::Exit;
        }
        
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Up:
                    updateSelection(selectedOption - 1);
                    break;
                    
                case sf::Keyboard::Down:
                    updateSelection(selectedOption + 1);
                    break;
                    
                case sf::Keyboard::Return:
                    window.close(); // Selection confirmed, close the menu
                    break;
                    
                case sf::Keyboard::Num1:
                    updateSelection(0);
                    break;
                    
                case sf::Keyboard::Num2:
                    updateSelection(1);
                    break;
                    
                case sf::Keyboard::Num3:
                    updateSelection(2);
                    break;
                    
                case sf::Keyboard::Num4:
                    updateSelection(3);
                    break;
                    
                case sf::Keyboard::Escape:
                    window.close();
                    selectedMode = SimulationMode::Exit;
                    break;
                    
                default:
                    break;
            }
        }
        
        // Mouse click handling
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            
            for (size_t i = 0; i < optionBackgrounds.size(); i++) {
                if (optionBackgrounds[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    updateSelection(i);
                    
                    // Double-click or single click on already selected item confirms selection
                    if (i == static_cast<size_t>(selectedOption)) {
                        window.close();
                    }
                    
                    break;
                }
            }
        }
    }
}

void StartupMenu::render() {
    window.clear(backgroundColor);
    
    // Draw backgrounds first
    for (const auto& bg : optionBackgrounds) {
        window.draw(bg);
    }
    
    // Draw description background
    window.draw(descriptionBackground);
    
    // Draw title and options
    window.draw(titleText);
    
    for (const auto& option : menuOptions) {
        window.draw(option);
    }
    
    // Draw the active description
    window.draw(descriptionTexts[selectedOption]);
    
    window.display();
}

SimulationMode StartupMenu::run() {
    while (window.isOpen()) {
        handleInput();
        render();
    }
    
    return selectedMode;
}