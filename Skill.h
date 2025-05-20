#ifndef SKILL_ENUMS_H
#define SKILL_ENUMS_H
using namespace std;
#include <string>

enum class SkillType {
    None,
    Slash,
    IceSlash,
    PoisonDart,
    Heal,
};

struct SkillData {  
    string name;
    int baseDamage;
    int critChancePercent;
    string statusEffect;
    int effectDuration;
    int healAmount;
};

static const SkillData skillTable[] = {
    {"Slash", 15, 20, "", 0, 0},
    {"Ice Slash", 12, 0, "Stun", 1, 0},
    {"Poison Dart", 9, 0, "Poison", 3, 0},
    {"Heal", 0, 0, "", 0, 30}
};

inline const SkillData& getSkillData(SkillType skill) {
    return skillTable[static_cast<int>(skill)];
};

#endif