#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

class MapManager {
private:
    // Grid dimensions
    static const int mapWidth = 10;
    static const int mapHeight = 10;
    const float tileSize = 60.0f;
    
    // Grid offset
    float gridOffsetX;
    float gridOffsetY;
    
    // Player position
    sf::Vector2f playerPosition;
    
    // Textures and sprites
    sf::Texture doorTexture;
    vector<sf::Sprite> doorSprites; // Multiple doors
    vector<sf::Vector2f> doorPositions; // Door positions for interaction
    
    sf::Texture playerTexture;
    sf::Sprite playerSprite;
    
    sf::Texture orbTexture;
    sf::Sprite orbSprite;
    vector<sf::Vector2f> healOrbPositions; // Healing orb positions
    
    // References
    GameManager* gameManager;
    sf::RenderWindow* window;
    sf::Font* font;
    
public:
    MapManager(GameManager* gm, sf::RenderWindow* win, sf::Font* fnt);
    ~MapManager();
    
    // Initialization
    void initialize();
    bool loadTextures();
    
    // Drawing
    void drawMap();
    void drawInteractionPrompt();
    
    // Player actions
    void movePlayer(float dx, float dy);
    sf::Vector2f getPlayerPosition() const { return playerPosition; }
    void setPlayerPosition(float x, float y);
    
    // Grid utilities
    bool isWithinBounds(float x, float y) const;
    
    // Getters
    float getGridOffsetX() const { return gridOffsetX; }
    float getGridOffsetY() const { return gridOffsetY; }
    float getTileSize() const { return tileSize; }
    const std::vector<sf::Sprite>& getDoorSprites() const { return doorSprites; }
    
    // Grid coordinate conversion
    sf::Vector2i pixelToGrid(float x, float y) const;
    sf::Vector2f gridToPixel(int x, int y) const;
    
    // Interaction methods
    bool isPlayerNearDoor() const;
    sf::Vector2f getNearestDoorPosition() const;
    int getNearestDoorIndex() const;
    bool isPlayerNearHealOrb() const;
    void removeHealOrb();
    
    // Map management
    void resetMap();
};

#endif