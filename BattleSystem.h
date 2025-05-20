#ifndef BATTLE_SYSTEM_H
#define BATTLE_SYSTEM_H

#include "Player.h"
#include "Enemy.h"
#include <SFML/Window/Event.hpp>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class BattleSystem {
private:
    Player* player = nullptr;
    Enemy* enemy = nullptr;
    bool playerTurn = true;
    int selectedAction = 0;
    vector<string> actionNames = {"Physical Attack", "Defend", "Magic Attack", "Use Skill"};
    
    void playerTurnPhase();
    void enemyTurnPhase();

public:
    BattleSystem() = default;
    
    // Battle setup and management
    void fight(Player& p, Enemy& e);
    void runTurn();
    bool isBattleOver() const;
    bool isPlayerVictorious() const;
    
    // Input handling
    bool handleInput(const sf::Event& event);
    
    // Action selection
    int getSelectedAction() const { return selectedAction; }
    void setSelectedAction(int action);
    void selectPreviousAction();
    void selectNextAction();
    
    // Player turn management
    bool isPlayerTurn() const { return playerTurn; }
    bool executeSelectedAction();
    
    // Getters
    Player* getPlayer() const { return player; }
    Enemy* getEnemy() const { return enemy; }
    const vector<string>& getActionNames() const { return actionNames; }
    
    // Reset battle state
    void reset();
};

#endif