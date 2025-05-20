#include "Character.h"
#include <iostream>
using namespace std;

Character::Character(string name, int maxHp) : name(name), maxHealth(maxHp), health(maxHp), isDefending(false), 
    isPoisoned(false), poisonTurns(0), poisonDamage(0), isStunned(false) {}

void Character::takeDamage(int amount) {
    if (isDefending) {
    amount = 0;
    isDefending = false;
    }
    health -= amount;
    if (health < 0) { health = 0;}
    cout << getName() << " takes " << amount << " damage! Health: " << health << "/" << maxHealth << endl;
}

bool Character::isAlive() const {
    return health > 0;
}

void Character::setDefenseState(bool state) {
    isDefending = state;
}

bool Character::isDefendingFunc() const {
    return isDefending;
}

void Character::applyPoison(int damage, int turns) {
    isPoisoned = true;
    poisonDamage = damage;
    poisonTurns = turns;
}

void Character::applyStun() {
    isStunned = true;
}

void Character::processStatusEffect() {
    if (isPoisoned && poisonTurns > 0) {
        health -= poisonDamage;
        poisonTurns--;
        if (poisonTurns == 0) {
            isPoisoned = false;
        }
    }
    if (isStunned) {
        isStunned = false;
    }
}

int Character::getHealth() const {
    return health;
}

int Character::getMaxHealth() const {
    return maxHealth;
}

string Character::getName() const {
    return name;
}

void Character::setHealth(int hp) {
    health = hp;
}