#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

class Game {
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text text;

public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void loadResources();
};

#endif
