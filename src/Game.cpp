#include "Game.hpp"
#include "FastNoiseLite.h"
#include "debug.hpp"
#include <random>
#include <set>
#include <unordered_map>
#include <algorithm>

<<<<<<< HEAD
Game::Game() 
    : window(sf::VideoMode(GameConfig::windowWidth, GameConfig::windowHeight), "MicroSociety"), clockGUI(700, 100) {
=======
// Constructor
Game::Game()
    : window(sf::VideoMode(GameConfig::windowWidth, GameConfig::windowHeight), "MicroSociety", sf::Style::Titlebar | sf::Style::Close),
      clockGUI(700, 100) {
#ifdef _WIN32
    ui.adjustLayout(window);
#endif

>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
    generateMap();
    npcs = generateNPCs();

    // Initialize market prices
    market.setPrice("wood", 10.0f);
    market.setPrice("stone", 10.0f);
    market.setPrice("bush", 5.0f);

    ui.updateNPCList(npcs); // Update UI with NPC list
}

<<<<<<< HEAD


=======
>>>>>>> c58c2ae0db671771a141dbe652525b0b4aaea4db
const std::vector<std::vector<std::unique_ptr<Tile>>>& Game::getTileMap() const {
    return tileMap;
}

bool Game::detectCollision(const NPC& npc) {
    int tileX = static_cast<int>(npc.getPosition().x / GameConfig::tileSize);
    int tileY = static_cast<int>(npc.getPosition().y / GameConfig::tileSize);

    if (tileX >= 0 && tileX < tileMap[0].size() && tileY >= 0 && tileY < tileMap.size()) {
        auto& tile = tileMap[tileY][tileX];
        return tile->hasObject() && npc.getSprite().getGlobalBounds().intersects(tile->getObjectBounds());
    }
    return false;
}

void Game::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
            if (event.type == sf::Event::Resized) ui.adjustLayout(window);

            ui.handleButtonClicks(window, event, npcs, timeManager, market);
            ui.handleNPCPanel(window, event, npcs);
            ui.handleStatsPanel(window, event);
            ui.handleOptionsEvents(window, event, *this);
        }

        deltaTime = clock.restart().asSeconds() * simulationSpeed;

        // Simulate NPCs and societal growth
        simulateNPCBehavior(deltaTime);
        simulateSocietalGrowth(deltaTime);

        timeManager.update(deltaTime);
        clockGUI.update(timeManager.getElapsedTime());

        ui.updateStatus(
            timeManager.getCurrentDay(),
            timeManager.getFormattedTime(),
            timeManager.getSocietyIteration()
        );

        window.clear();
        render();
        ui.render(window, market, npcs);
        clockGUI.render(window, isClockVisible);
        debugConsole.render(window);
        window.display();
    }
}

void Game::simulateNPCBehavior(float deltaTime) {
    for (auto& npc : npcs) {
        evaluateNPCState(npc);

        if (npc.getEnergy() <= 20.0f) {
            npc.performAction(std::make_unique<RegenerateEnergyAction>(), tileMap);
        } else if (npc.getMoney() >= 50.0f) {
            npc.performAction(std::make_unique<UpgradeHouseAction>(), tileMap);
        } else {
            performPathfinding(npc);
        }

        npc.update(deltaTime);
    }
}

void Game::evaluateNPCState(NPC& npc) {
    if (npc.getEnergy() <= 0.0f) {
        getDebugConsole().log("NPC", npc.getName() + " ran out of energy and died.");
        // Handle NPC death (remove or reset state)
    }
}

void Game::simulateSocietalGrowth(float deltaTime) {
    // Example societal growth logic: increase market prices as demand rises
    static float timeAccumulator = 0.0f;
    timeAccumulator += deltaTime;

    if (timeAccumulator >= 60.0f) { // Adjust market prices every 60 seconds
        market.adjustPrices(1.05f); // Increase all prices by 5%
        getDebugConsole().log("Society", "Market prices adjusted due to societal growth.");
        timeAccumulator = 0.0f;
    }
}

void Game::performPathfinding(NPC& npc) {
    // Basic random pathfinding logic for NPCs
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(-1, 1);

    int moveX = dist(gen);
    int moveY = dist(gen);

    sf::Vector2f newPosition = npc.getPosition() + sf::Vector2f(moveX * GameConfig::tileSize, moveY * GameConfig::tileSize);
    npc.setPosition(newPosition.x, newPosition.y);
}

void Game::generateMap() {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.1f);
    noise.SetSeed(static_cast<int>(time(nullptr)));

    std::vector<sf::Texture> grassTextures(3), stoneTextures(3), flowerTextures(5);
    std::vector<sf::Texture> bushTextures(2), rockTextures(3), treeTextures(3);
    std::vector<sf::Texture> houseTextures(3), marketTextures(3);

    for (int i = 0; i < 3; ++i) {
        grassTextures[i].loadFromFile("../assets/tiles/grass/grass" + std::to_string(i + 1) + ".png");
        stoneTextures[i].loadFromFile("../assets/tiles/stone/stone" + std::to_string(i + 1) + ".png");
        houseTextures[i].loadFromFile("../assets/objects/house" + std::to_string(i + 1) + ".png");
        marketTextures[i].loadFromFile("../assets/objects/market" + std::to_string(i + 1) + ".png");
    }

    for (int i = 0; i < 2; ++i) {
        bushTextures[i].loadFromFile("../assets/objects/bush" + std::to_string(i + 1) + ".png");
    }

    for (int i = 0; i < 5; ++i) {
        flowerTextures[i].loadFromFile("../assets/tiles/flower/flower" + std::to_string(i + 1) + ".png");
    }

    tileMap.resize(GameConfig::mapHeight, std::vector<std::unique_ptr<Tile>>(GameConfig::mapWidth));

    for (int i = 0; i < GameConfig::mapHeight; ++i) {
        for (int j = 0; j < GameConfig::mapWidth; ++j) {
            float noiseValue = noise.GetNoise(static_cast<float>(i), static_cast<float>(j));
            noiseValue = (noiseValue + 1.0f) / 2.0f;

            if (noiseValue < 0.2f) {
                tileMap[i][j] = std::make_unique<FlowerTile>(flowerTextures[rand() % 5]);
            } else if (noiseValue < 0.6f) {
                tileMap[i][j] = std::make_unique<GrassTile>(grassTextures[rand() % 3]);
            } else {
                tileMap[i][j] = std::make_unique<StoneTile>(stoneTextures[rand() % 3]);
            }

            tileMap[i][j]->setPosition(j * GameConfig::tileSize, i * GameConfig::tileSize);

            int objectChance = rand() % 100;
            if (auto grassTile = dynamic_cast<GrassTile*>(tileMap[i][j].get())) {
                if (objectChance < 20) grassTile->placeObject(std::make_unique<Tree>(treeTextures[rand() % 3]));
                else if (objectChance < 30) grassTile->placeObject(std::make_unique<Bush>(bushTextures[rand() % 2]));
            } else if (auto stoneTile = dynamic_cast<StoneTile*>(tileMap[i][j].get())) {
                if (objectChance < 20) stoneTile->placeObject(std::make_unique<Rock>(rockTextures[rand() % 3]));
            }
        }
    }

    std::set<std::pair<int, int>> occupiedPositions;
    for (int i = 0; i < GameConfig::npcCount; ++i) {
        int houseX, houseY;
        do {
            houseX = rand() % GameConfig::mapWidth;
            houseY = rand() % GameConfig::mapHeight;
        } while (occupiedPositions.count({houseX, houseY}) || tileMap[houseY][houseX]->hasObject());

        occupiedPositions.insert({houseX, houseY});

        sf::Color houseColor(rand() % 256, rand() % 256, rand() % 256);
        auto house = std::make_unique<House>(houseTextures[i % 3]);
        house->getSprite().setColor(houseColor);

        tileMap[houseY][houseX]->placeObject(std::move(house));
    }

    int marketCount = 2 + rand() % 2;
    for (int m = 0; m < marketCount; ++m) {
        int marketX, marketY;
        do {
            marketX = rand() % GameConfig::mapWidth;
            marketY = rand() % GameConfig::mapHeight;
        } while (occupiedPositions.count({marketX, marketY}) || tileMap[marketY][marketX]->hasObject());

        occupiedPositions.insert({marketX, marketY});
        tileMap[marketY][marketX]->placeObject(std::make_unique<Market>(marketTextures[m % 3]));
    }
}

std::vector<NPC> Game::generateNPCs() const {
    std::vector<NPC> npcs;
    std::set<std::pair<int, int>> occupiedPositions;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(0, GameConfig::mapWidth - 1);
    std::uniform_int_distribution<> distY(0, GameConfig::mapHeight - 1);
    std::uniform_int_distribution<> statDist(50, 150);

    for (int i = 0; i < GameConfig::npcCount; ++i) {
        int x, y;
        do {
            x = distX(gen);
            y = distY(gen);
        } while (occupiedPositions.count({x, y}));

        occupiedPositions.insert({x, y});

        sf::Color npcColor(rand() % 256, rand() % 256, rand() % 256);
        NPC npc("NPC" + std::to_string(i + 1), statDist(gen), statDist(gen), statDist(gen), 150.0f, 10, statDist(gen));
        npc.setTexture(playerTexture, npcColor);
        npc.setPosition(x * GameConfig::tileSize, y * GameConfig::tileSize);

        npcs.push_back(npc);
    }
    return npcs;
}

void Game::render() {
    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            tile->draw(window);
        }
    }

    for (const auto& npc : npcs) {
        npc.draw(window);
    }

    if (showTileBorders) drawTileBorders();
}

void Game::drawTileBorders() {
    for (int i = 0; i < tileMap.size(); ++i) {
        for (int j = 0; j < tileMap[i].size(); ++j) {
            sf::RectangleShape border(sf::Vector2f(GameConfig::tileSize, GameConfig::tileSize));
            border.setPosition(j * GameConfig::tileSize, i * GameConfig::tileSize);
            border.setOutlineThickness(1);
            border.setOutlineColor(sf::Color::Black);
            border.setFillColor(sf::Color::Transparent);
            window.draw(border);
        }
    }
}

void Game::resetSimulation() {
    generateMap();
    npcs = generateNPCs();
    ui.updateNPCList(npcs);
    getDebugConsole().log("Options", "Simulation reset.");
}

void Game::toggleTileBorders() {
    showTileBorders = !showTileBorders;
    getDebugConsole().log("Options", "Tile borders toggled: " + std::string(showTileBorders ? "ON" : "OFF"));
}

void Game::setSimulationSpeed(float speedFactor) {
    simulationSpeed = speedFactor;
    getDebugConsole().log("Options", "Simulation speed set to: " + std::to_string(speedFactor));
}
