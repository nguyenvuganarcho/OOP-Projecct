#include "Enemy.h"
using namespace std;
#include <iostream>

Enemy::Enemy(string name, Difficulty diff, AbilityType abil, ResistanceType res, string reward, int exp) : 
    Character(name, 0), difficulty(diff), ability(abil), resistance(res), rewardSkill(reward), rewardExp(exp) {
        initializeByDifficulty();
        health = maxHealth;
        rewardExp = exp;
        assignAbilities();
    }

void Enemy::initializeByDifficulty() {
    switch (difficulty) {
        case Difficulty::Easy:
        maxHealth = 50;
        baseDamage = 8;
        rewardExp = 10;
        break;
        case Difficulty::Medium:
        maxHealth = 75;
        baseDamage = 13;
        rewardExp = 25;
        break;
        case Difficulty::Hard:
        maxHealth = 100;
        baseDamage = 18;
        rewardExp = 35;
        break;
    }
}

void Enemy::assignAbilities() {
    if (difficulty == Difficulty::Easy) {
        abilities.push_back(AbilityType::NormalAttack);
    }
    else if (difficulty == Difficulty::Medium) {
        abilities.push_back(AbilityType::NormalAttack);
        abilities.push_back(AbilityType::Poison);
    }
    else if (difficulty == Difficulty::Hard) {
        abilities.push_back(AbilityType::NormalAttack);
        abilities.push_back(AbilityType::Poison);
        abilities.push_back(AbilityType::Stun);
    }

}

void Enemy::useAbility(Character& target) {
    int randomIndex = rand() % abilities.size();
    AbilityType selectedAbility = abilities[randomIndex];
    cout << getName() << " uses ability on " << target.getName() << endl;
    cout << "Enemy attacking target at address: " << &target << endl;
    switch (selectedAbility)
    {
    case AbilityType::NormalAttack:
    cout << "Dealing damage: " << baseDamage << endl;    
    target.takeDamage(baseDamage);
        break;
    case AbilityType::Stun:
        target.applyStun();
        break;
    case AbilityType::Poison:
        target.applyPoison(5, 3);
        break;
    case AbilityType::Defend:
        setDefenseState(true);  
        break;
    }
}


void Enemy::attack(Character& target) {
    useAbility(target);
}

void Enemy::defend() {
    setDefenseState(true);    
}

Difficulty Enemy::getDifficulty() const {
    return difficulty;
}

AbilityType Enemy::getAbility() const {
    return ability;
}

ResistanceType Enemy::getResistance() const {
    return resistance;
}

string Enemy::getRewardSkill() const {
    return rewardSkill;
}

int Enemy::getBaseDamage() const {
    return baseDamage;
}

int Enemy::getMaxHealth() const {
    return maxHealth;
}

int Enemy::getRewardExp() const {
    return rewardExp;
}

