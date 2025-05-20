#include "BattleSystem.h"

using namespace std;

void BattleSystem::fight(Player& p, Enemy& e) {
    player = &p;
    enemy = &e;
    cout << "Player instance address: " << player << endl;
    cout << "Enemy instance address: " << enemy << endl;
    playerTurn = true;
    selectedAction = 0; 
    player->setInBattle(true);
}

void BattleSystem::playerTurnPhase() {
    player->setDefenseState(false);
    player->attack(*enemy);
}

void BattleSystem::enemyTurnPhase() {
    enemy->useAbility(*player);
    cout << "enemy dealt damage: " << player->getHealth() << "/" << player->getMaxHealth() << endl;
}

// Super simplified runTurn - just enemy attacking player
void BattleSystem::runTurn() {
    if (!player || !enemy) {
        return;
    }

    if (playerTurn) {
        playerTurnPhase();
    } else {
        enemyTurnPhase();
        
    }

    playerTurn = !playerTurn;
}
bool BattleSystem::isBattleOver() const {
    if (!player || !enemy) return true;

    return (player->getHealth() <= 0 || enemy->getHealth() <= 0);
}

bool BattleSystem::isPlayerVictorious() const {
    if (!enemy) return false;
    return enemy->getHealth() <= 0;
}

bool BattleSystem::handleInput(const sf::Event& event) {
    // Only handle input during player's turn
    if (!playerTurn) return false;
    
    // If this is a key press event
    if (event.type == sf::Event::KeyPressed) {
        // Navigate battle options with arrow keys
        if (event.key.code == sf::Keyboard::Up) {
            selectPreviousAction();
            std::cout << "Selected action: " << actionNames[selectedAction] << std::endl;
            return true;
        }
        else if (event.key.code == sf::Keyboard::Down) {
            selectNextAction();
            std::cout << "Selected action: " << actionNames[selectedAction] << std::endl;
            return true;
        }
        // Number keys for quick selection (1-4)
        else if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num5) {
            int actionIndex = event.key.code - sf::Keyboard::Num1;
            setSelectedAction(actionIndex);
            std::cout << "Selected action: " << actionNames[selectedAction] << std::endl;
            return true;
        }
        // Confirm action with Enter or Space
        else if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Return) {
            // Execute the selected action
            if (executeSelectedAction()) {
                std::cout << "Action executed: " << actionNames[selectedAction] << std::endl;
                return true;
            }
        }
    }
    
    return false; // No action was taken
}

void BattleSystem::setSelectedAction(int action) {
    // Ensure the action is within bounds (0-4)
    if (action >= 0 && action <static_cast<int>(actionNames.size())) {
        selectedAction = action;
    }
}

void BattleSystem::selectPreviousAction() {
    if (selectedAction > 0) {
        selectedAction--;
    }
}

void BattleSystem::selectNextAction() {
    if (selectedAction < static_cast<int>(actionNames.size()) - 1) {
        selectedAction++;
    }
}

// Update executeSelectedAction in BattleSystem.cpp
bool BattleSystem::executeSelectedAction() {
    if (!player || !enemy || !playerTurn) {
        cout << "Cannot execute action: missing player/enemy or not player's turn" << std::endl;
        return false;
    }
    
    cout << "Executing action: " << actionNames[selectedAction] << std::endl;
    
    // Execute different actions based on selection
    switch (selectedAction) {
        case 0: // Physical Attack
            cout << player->getName() << " uses Physical Attack!" << std::endl;
            player->physicalAttack(*enemy);
            cout << "Dealt damage! Enemy health: " << enemy->getHealth() << "/" 
                      << enemy->getMaxHealth() << std::endl;
            break;
            
        case 1: // Defend
            cout << player->getName() << " defends!" << std::endl;
            player->defend();
            break;
            
        case 2: // Magic Attack
            cout << player->getName() << " uses Magic Attack!" << std::endl;
            player->magicalAttack(*enemy);
            cout << "Dealt damage! Enemy health: " << enemy->getHealth() << "/" 
                      << enemy->getMaxHealth() << std::endl;
            break;
            
        case 3: // Use Skill
            cout << player->getName() << " tries to use a skill!" << std::endl;
            player->useSkill(*enemy);
            cout << "Used skill! Enemy health: " << enemy->getHealth() << "/" 
                      << enemy->getMaxHealth() << std::endl;
            break;
            
        default:
            cout << "Invalid action selected" << std::endl;
            return false;
    }
    
    // Check if battle is over after action
    if (isBattleOver()) {
        std::cout << "Battle is over after player action!" << std::endl;
        return true;
    }
    
    // Switch to enemy turn - THIS IS CRITICAL
    cout << "Switching to enemy turn" << std::endl;
    playerTurn = false;
    runTurn();
    
    return true; // Action executed successfully
}

void BattleSystem::reset() {
    player = nullptr;
    enemy = nullptr;
    playerTurn = true;
    selectedAction = 0;
}