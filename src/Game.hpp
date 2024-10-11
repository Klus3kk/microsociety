#ifndef GAME_HPP
#define GAME_HPP 

#include <SFML/Graphics.hpp>

class Game {
protected:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text text;
public:
    Game();
    void run();
private:
    void loadResources();
};


#endif