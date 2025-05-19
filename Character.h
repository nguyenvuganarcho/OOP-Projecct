#ifndef CHARACTER_H
#define CHARACTER_H
using namespace std;
#include <string>

class Character {
    protected:
    string name;
    bool isDefending;
    bool isPoisoned;
    int poisonTurns;
    int poisonDamage;
    bool isStunned;
    int health;
    int maxHealth;

    public:
    Character (string name, int maxHp);
    virtual ~Character() = default;

    virtual void attack(Character& target) = 0;  // Abstract attack method
    virtual void takeDamage(int amount);    // Damage-taking logic
    bool isAlive() const;

    void setDefenseState(bool state);   // Set defense mode (reduce damage)
    bool isDefendingFunc() const;

    void applyPoison(int damage, int turns);    // Apply poison status
    void applyStun();    // Apply stun status
    void processStatusEffect();

    int getHealth() const;
    int getMaxHealth() const;
    string getName() const;

    void setHealth(int newHp);
 
};

#endif 