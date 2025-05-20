#ifndef PLAYER_H
#define PLAYER_H
using namespace std;
#include "Character.h"
#include "Skill.h"
#include <vector>
#include <string>

class Player : public Character {
    private:
    int level;
    int exp;
    int maxHealth;
    int health;
    int baseDamage;
    static const int expToLevel = 35;  // EXP threshold
    SkillType equippedSkill;
    vector<SkillType> skillInventory;

    bool inBattle = false;

    public:
    Player(const string& name, int hp);

    void gainExp(int amount);
    void checkLevelUp();
    void level_Up();
    void resetProgress();
    
    void physicalAttack(Character& target);
    void magicalAttack(Character& target);
    void attack(Character& target) override;
    void defend();


    void addToInventory(SkillType skill);
    void manageEquippedSkills();
    void useSkill(Character& target);

    int getLevel() const;
    int getExp() const;
    int getMaxHealth() const;
    //int getHealth() const;
    
    void setInBattle(bool battleState);
    

};

#endif