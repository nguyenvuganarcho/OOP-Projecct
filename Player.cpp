#include "Player.h"
#include "Skill.h"
#include <cstdlib>  // for rand()
#include <algorithm>
#include <iostream>
using namespace std;


Player::Player(const std::string& name, int hp)
    : Character(name, hp), level(1), exp(0), maxHealth(hp), health(hp), baseDamage(10), equippedSkill(SkillType::None), inBattle(false) {
    // Initialize equipped skills empty (no extra skills at start)
}

void Player::physicalAttack(Character& target) {
    int damage = baseDamage + 5;
    target.takeDamage(damage);
}

void Player::magicalAttack(Character& target) {
    int damage = baseDamage + 5;
    target.takeDamage(damage);
}

void Player::defend() {
    setDefenseState(true);
}

void Player::attack(Character& target) {
    int choice = 1;
    switch (choice) {
        case 1:
            physicalAttack(target);
            break;
        case 2:
            magicalAttack(target);
            break;
        case 3:
            defend();
            break;
        case 4:
            if (equippedSkill != SkillType::None)
                useSkill(target);
            break;
}
}

void Player::useSkill(Character& target) {
    if (equippedSkill == SkillType::None) {
        return;
    }
    const SkillData& data = getSkillData(equippedSkill);

    if (equippedSkill == SkillType::Heal) {
        health += data.healAmount;
        if (health > maxHealth) health = maxHealth;
        return;
    }

    int damage = data.baseDamage;
    if (equippedSkill == SkillType::Slash) {
        int roll = rand() % 100;
        if (roll < data.critChancePercent) {
            damage *= 3;
        }
    }

    if (!data.statusEffect.empty()) {
        if (data.statusEffect == "Poison") {
            target.applyPoison(damage, data.effectDuration);
        }
        else if (data.statusEffect == "Stun") {
            target.applyStun();
        }
    }
}

void Player::addToInventory(SkillType newSkill) {
    if (newSkill == SkillType::None) {
        return;
    }

    if(std::find(skillInventory.begin(), skillInventory.end(), newSkill) == skillInventory.end()) {
        skillInventory.push_back(newSkill);
    }
}

void Player::manageEquippedSkills() {
    if(inBattle) {
        return;
    }

    if(skillInventory.empty()) {
        return;
    }


}

void Player::gainExp(int amount) {
    exp += amount;
}

void Player::checkLevelUp() {
    while (exp >= expToLevel) {
        exp -= expToLevel;
        level_Up();
    }
}

void Player::level_Up() {
    level++;
    maxHealth += 35;
}

void Player::resetProgress() {
    health = maxHealth;
    exp = 0;
    level = 1;
    skillInventory.clear();
    equippedSkill = SkillType::None;
    cout << "Progress reset!" << endl;
}

int Player::getLevel() const {
    return level;
}

int Player::getExp() const {
    return exp;
}

int Player::getMaxHealth() const {
    return maxHealth;
}

//"/int Player::getHealth() const {
   // return health;
//}"

void Player::setInBattle(bool battleState) {
    inBattle = battleState;
}