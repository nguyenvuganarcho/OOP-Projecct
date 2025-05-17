#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "Enums.h"

// Forward declarations
class MapManager;

class GameManager {
private:
    // Window
    sf::RenderWindow window;
    
    // Game state
    GameState currentState;
    
    // Resources
    sf::Font font;
    
    // Menu variables
    int selectedMenuOption;
    std::vector<std::string> menuOptions;
    
    // Map manager
    MapManager* mapManager;
    
    // Helper methods
    void loadResources();
    
public:
    GameManager();
    ~GameManager();
    
    // Core game methods
    void run();
    void processEvents();
    void update(float deltaTime);
    void render();
    
    // Screen drawing methods
    void drawMainMenu();
    
    // Transition methods
    void startTransition(GameState newState);
    
    // Getters
    sf::RenderWindow& getWindow() { return window; }
    sf::Font& getFont() { return font; }
};

#endif