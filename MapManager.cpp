#include "MapManager.h"
#include "GameManager.h"
#include "Player.h"
#include "Enums.h"
#include <iostream>
#include <cmath>

using namespace std;

MapManager::MapManager(GameManager* gm, sf::RenderWindow* win, sf::Font* fnt) :
    gameManager(gm),
    window(win),
    font(fnt)
{
    // Calculate grid offsets to center the grid in the window
    gridOffsetX = (window->getSize().x - mapWidth * tileSize) / 2;
    gridOffsetY = 60; // Start grid right after the stats bar
    
    // Initialize player position at the center of the grid
    playerPosition.x = gridOffsetX + 5 * tileSize + tileSize / 4;
    playerPosition.y = gridOffsetY + 5 * tileSize + tileSize / 4;
}

MapManager::~MapManager() {
}

void MapManager::initialize() {
    // Load textures for game objects
    if (!loadTextures()) {
        std::cerr << "Failed to load some textures. Images may not display correctly." << std::endl;
    }
    
    // Setup door sprites - create three doors positioned at the top of the grid
    doorSprites.clear();
    
    // Calculate scale based on tile size (we want doors to fit in a single tile)
    float doorScale = tileSize * 1.25 / std::max(doorTexture.getSize().x, doorTexture.getSize().y);
    
    // Create three door sprites at different locations
    sf::Sprite doorSprite1(doorTexture);
    doorSprite1.setScale(doorScale, doorScale);
    doorSprite1.setPosition(
        gridOffsetX + 2 * tileSize,
        gridOffsetY + 1 * tileSize
    );
    doorPositions.push_back(sf::Vector2f(gridOffsetX + 2 * tileSize, gridOffsetY + 1 * tileSize));
    doorSprites.push_back(doorSprite1);
    
    sf::Sprite doorSprite2(doorTexture);
    doorSprite2.setScale(doorScale, doorScale);
    doorSprite2.setPosition(
        gridOffsetX + 4 * tileSize,
        gridOffsetY + 1 * tileSize
    );
    doorPositions.push_back(sf::Vector2f(gridOffsetX + 4 * tileSize, gridOffsetY + 1 * tileSize));
    doorSprites.push_back(doorSprite2);
    
    sf::Sprite doorSprite3(doorTexture);
    doorSprite3.setScale(doorScale, doorScale);
    doorSprite3.setPosition(
        gridOffsetX + 6 * tileSize,
        gridOffsetY + 1 * tileSize
    );
    doorPositions.push_back(sf::Vector2f(gridOffsetX + 7 * tileSize, gridOffsetY + 1 * tileSize));
    doorSprites.push_back(doorSprite3);
    
    // Setup player sprite
    playerSprite.setTexture(playerTexture);
    float playerScale = tileSize * 1.5 / std::max(playerTexture.getSize().x, playerTexture.getSize().y);
    playerSprite.setScale(playerScale, playerScale);
    
    // Setup orb sprite
    orbSprite.setTexture(orbTexture);
    float orbScale = tileSize / 1.5 / std::max(orbTexture.getSize().x, orbTexture.getSize().y);
    orbSprite.setScale(orbScale, orbScale);
    orbSprite.setPosition(
        gridOffsetX + 7 * tileSize,
        gridOffsetY + 5 * tileSize
    );
    
    // Initialize heal orb positions
    healOrbPositions.push_back(sf::Vector2f(gridOffsetX + 7 * tileSize, gridOffsetY + 5 * tileSize));
}

bool MapManager::loadTextures() {
    bool success = true;
    
    // Try to load door texture from file
   
        if (!doorTexture.loadFromFile("/home/nguyenvu343/sfml/Image and fonts/door3.png")) {
            cerr << "Failed to load door texture from backup path" << std::endl;
            success = false;
        } else {
            cout << "Successfully loaded door texture from backup path" << std::endl;
        }
    
    // Try to load player texture

        if (!playerTexture.loadFromFile("/home/nguyenvu343/sfml/Image and fonts/0_Fallen_Angels_Idle_008.png")) {
            cerr << "Failed to load player texture from backup path" << std::endl;
            success = false;
        } else {
            cout << "Successfully loaded player texture from backup path" << std::endl;
        }

    
    // Try to load orb texture
  
        if (!orbTexture.loadFromFile("/home/nguyenvu343/sfml/Image and fonts/HP_Bonus_01.png")) {
            cerr << "Failed to load orb texture from backup path" << std::endl;
            success = false;
        } else {
            cout << "Successfully loaded orb texture from backup path" << std::endl;
        }
    
    return success;
}

void MapManager::drawMap() {
    // First get the player from GameManager
    Player* player = gameManager->getPlayer();
    
    // Player stats with brighter background - using getters from Player class
    std::string playerStats = player->getName() + 
                             " | Level: " + std::to_string(player->getLevel()) + 
                             " | HP: " + std::to_string(player->getHealth()) + "/" + std::to_string(player->getMaxHealth()) + 
                             " | Exp: " + std::to_string(player->getExp()) + "/35";
    
    sf::Text statsText(playerStats, *font, 20);
    statsText.setFillColor(sf::Color::White);
    statsText.setPosition(10, 20);
    
    // Create a dark blue rectangle behind stats
    sf::RectangleShape statsBg(sf::Vector2f(window->getSize().x - 20, 30));
    statsBg.setFillColor(sf::Color(20, 40, 80, 255));
    statsBg.setPosition(10, 20);
    
    window->draw(statsBg);
    window->draw(statsText);
    
    // Draw grid outline with more visible color
    sf::RectangleShape gridOutline(sf::Vector2f(mapWidth * tileSize + 20, mapHeight * tileSize + 20));
    gridOutline.setFillColor(sf::Color(120, 100, 70, 255));
    gridOutline.setOutlineColor(sf::Color::White);
    gridOutline.setOutlineThickness(2);
    gridOutline.setPosition(gridOffsetX - 10, gridOffsetY - 10);
    window->draw(gridOutline);
    
    // Draw grid tiles with much brighter colors
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            sf::RectangleShape tile(sf::Vector2f(tileSize - 2, tileSize - 2));
            tile.setFillColor(sf::Color(80, 130, 230, 255));
            tile.setOutlineColor(sf::Color(150, 180, 255, 255));
            tile.setOutlineThickness(1);
            tile.setPosition(gridOffsetX + x * tileSize + 1, gridOffsetY + y * tileSize + 1);
            window->draw(tile);
        }
    }
    
    // Draw all doors
    for (size_t i = 0; i < doorSprites.size(); i++) {
        // If we have a boss door (last door), make it special
        if (i == doorSprites.size() - 1) {
            // Check if boss is unlocked through the player level
            if (player && player->getLevel() >= 3) {
                // Boss door is a different color
                doorSprites[i].setColor(sf::Color(255, 100, 100)); // Reddish tint
            } else {
                // Boss door is locked (gray)
                doorSprites[i].setColor(sf::Color(100, 100, 100)); // Gray tint
            }
        }
        
        window->draw(doorSprites[i]);
    }
    
    // Draw player sprite
    playerSprite.setPosition(playerPosition.x, playerPosition.y);
    window->draw(playerSprite);
    
    // Draw all healing orbs
    for (auto& orbPos : healOrbPositions) {
        orbSprite.setPosition(orbPos.x, orbPos.y);
        window->draw(orbSprite);
    }
    
    // Navigation instructions with brighter color and background
    sf::Text controlsText("Arrow Keys: Move | I: Inventory | Space: Interact", *font, 20);
    controlsText.setFillColor(sf::Color::White);
    
    sf::RectangleShape controlsBg(sf::Vector2f(controlsText.getGlobalBounds().width + 20, 30));
    controlsBg.setFillColor(sf::Color(30, 60, 120, 200));
    controlsBg.setPosition(
        (window->getSize().x - controlsText.getGlobalBounds().width) / 2 - 10,
        gridOffsetY + mapHeight * tileSize + 20
    );
    
    controlsText.setPosition(
        (window->getSize().x - controlsText.getGlobalBounds().width) / 2,
        gridOffsetY + mapHeight * tileSize + 25
    );
    
    window->draw(controlsBg);
    window->draw(controlsText);
    
    // Calculate grid coordinates (which tile the player is on)
    sf::Vector2i gridPos = pixelToGrid(playerPosition.x, playerPosition.y);
    
    // Debug info with grid position
    sf::Text posText("Position: (" + std::to_string(static_cast<int>(playerPosition.x)) + ", " + 
                    to_string(static_cast<int>(playerPosition.y)) + ") | Grid: (" +
                    to_string(gridPos.x) + ", " + std::to_string(gridPos.y) + ")",
                    *font, 14);
    posText.setFillColor(sf::Color::White);
    
    // Create a background for the position text
    sf::RectangleShape posTextBg(sf::Vector2f(posText.getGlobalBounds().width + 20, 20));
    posTextBg.setFillColor(sf::Color(30, 30, 60, 200));
    posTextBg.setPosition(10, window->getSize().y - 25);
    
    window->draw(posTextBg);
    posText.setPosition(20, window->getSize().y - 25);
    window->draw(posText);
}

void MapManager::drawInteractionPrompt() {
    if (isPlayerNearDoor()) {
        sf::Text interactText("Press Space to enter door", *font, 18);
        interactText.setFillColor(sf::Color::Yellow);
        interactText.setPosition(
            (window->getSize().x - interactText.getGlobalBounds().width) / 2,
            window->getSize().y - 60
        );
        window->draw(interactText);
    }
    else if (isPlayerNearHealOrb()) {
        sf::Text interactText("Press Space to collect healing orb", *font, 18);
        interactText.setFillColor(sf::Color::Green);
        interactText.setPosition(
            (window->getSize().x - interactText.getGlobalBounds().width) / 2,
            window->getSize().y - 60
        );
        window->draw(interactText);
    }
}

void MapManager::movePlayer(float dx, float dy) {
    // Calculate potential new position
    sf::Vector2f newPosition = playerPosition;
    newPosition.x += dx;
    newPosition.y += dy;
    
    // Only update if within bounds
    if (isWithinBounds(newPosition.x, newPosition.y)) {
        playerPosition = newPosition;
    } else {
        // If moving out of bounds, still allow movement along the boundary
        if (isWithinBounds(newPosition.x, playerPosition.y)) {
            playerPosition.x = newPosition.x;
        }
        if (isWithinBounds(playerPosition.x, newPosition.y)) {
            playerPosition.y = newPosition.y;
        }
    }
}

void MapManager::setPlayerPosition(float x, float y) {
    if (isWithinBounds(x, y)) {
        playerPosition.x = x;
        playerPosition.y = y;
    }
}

bool MapManager::isWithinBounds(float x, float y) const {
    // Calculate the boundaries of the grid
    const float minX = gridOffsetX;
    const float maxX = gridOffsetX + mapWidth * tileSize - tileSize/2;
    const float minY = gridOffsetY;
    const float maxY = gridOffsetY + mapHeight * tileSize - tileSize/2;
    
    return (x >= minX && x <= maxX && y >= minY && y <= maxY);
}

sf::Vector2i MapManager::pixelToGrid(float x, float y) const {
    int gridX = static_cast<int>((x - gridOffsetX) / tileSize);
    int gridY = static_cast<int>((y - gridOffsetY) / tileSize);
    
    // Clamp to valid grid coordinates
    gridX = std::max(0, std::min(gridX, mapWidth - 1));
    gridY = std::max(0, std::min(gridY, mapHeight - 1));
    
    return sf::Vector2i(gridX, gridY);
}

sf::Vector2f MapManager::gridToPixel(int x, int y) const {
    float pixelX = gridOffsetX + x * tileSize + tileSize / 4;
    float pixelY = gridOffsetY + y * tileSize + tileSize / 4;
    
    return sf::Vector2f(pixelX, pixelY);
}

bool MapManager::isPlayerNearDoor() const {
    // Check if player is near any door
    const float interactionDistance = tileSize * 1.5f; // Adjust based on your needs
    
    for (const auto& doorPos : doorPositions) {
        float distance = std::sqrt(
            std::pow(playerPosition.x - doorPos.x, 2) + 
            std::pow(playerPosition.y - doorPos.y, 2)
        );
        
        if (distance < interactionDistance) {
            return true;
        }
    }
    
    return false;
}

sf::Vector2f MapManager::getNearestDoorPosition() const {
    const float interactionDistance = tileSize * 1.5f;
    float shortestDistance = interactionDistance;
    sf::Vector2f nearestDoor;
    bool foundDoor = false;
    
    for (const auto& doorPos : doorPositions) {
        float distance = std::sqrt(
            pow(playerPosition.x - doorPos.x, 2) + 
            pow(playerPosition.y - doorPos.y, 2)
        );
        
        if (distance < shortestDistance) {
            shortestDistance = distance;
            nearestDoor = doorPos;
            foundDoor = true;
        }
    }
    
    return foundDoor ? nearestDoor : sf::Vector2f(0, 0);
}

int MapManager::getNearestDoorIndex() const {
    const float interactionDistance = tileSize * 1.5f;
    float shortestDistance = interactionDistance;
    int nearestIndex = -1;
    
    for (size_t i = 0; i < doorPositions.size(); i++) {
        float distance = std::sqrt(
            pow(playerPosition.x - doorPositions[i].x, 2) + 
            pow(playerPosition.y - doorPositions[i].y, 2)
        );
        
        if (distance < shortestDistance) {
            shortestDistance = distance;
            nearestIndex = static_cast<int>(i);
        }
    }
    
    return nearestIndex;
}

bool MapManager::isPlayerNearHealOrb() const {
    // Using the orb sprite position
    const float interactionDistance = tileSize * 1.2f;
    
    for (const auto& orbPos : healOrbPositions) {
        float distance = std::sqrt(
            pow(playerPosition.x - orbPos.x, 2) + 
            pow(playerPosition.y - orbPos.y, 2)
        );
        
        if (distance < interactionDistance) {
            return true;
        }
    }
    
    return false;
}

void MapManager::removeHealOrb() {
    // Find the closest orb to remove
    const float interactionDistance = tileSize * 1.2f;
    float shortestDistance = interactionDistance;
    size_t indexToRemove = healOrbPositions.size(); // Invalid index by default
    
    for (size_t i = 0; i < healOrbPositions.size(); i++) {
        float distance = std::sqrt(
            pow(playerPosition.x - healOrbPositions[i].x, 2) + 
            pow(playerPosition.y - healOrbPositions[i].y, 2)
        );
        
        if (distance < shortestDistance) {
            shortestDistance = distance;
            indexToRemove = i;
        }
    }
    
    // Remove the orb if a valid one was found
    if (indexToRemove < healOrbPositions.size()) {
        healOrbPositions.erase(healOrbPositions.begin() + indexToRemove);
    }
}

void MapManager::resetMap() {
    // Reset player position to center
    playerPosition.x = gridOffsetX + 5 * tileSize + tileSize / 4;
    playerPosition.y = gridOffsetY + 5 * tileSize + tileSize / 4;
    
    // Reset healing orbs
    healOrbPositions.clear();
    
    // Add a heal orb
    healOrbPositions.push_back(sf::Vector2f(gridOffsetX + 7 * tileSize, gridOffsetY + 5 * tileSize));
}