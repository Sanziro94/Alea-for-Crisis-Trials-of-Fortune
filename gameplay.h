#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdbool.h>

// Definizioni condivise
typedef enum { STAT_HP, STAT_ATK, STAT_DEF, STAT_SPD } StatIndex;
typedef enum { HEAL, DAMAGE, BUFF_ATTACK, BUFF_DEFENSE, BUFF_SPEED } ObjectType;
typedef enum { SKILL_NORMAL_DAMAGE, SKILL_PIERCING, SKILL_VAMPIRISM, SKILL_BUFF } SkillType;

typedef struct {
    const char* name;
    ObjectType type;
    int value;
} Objects;

typedef struct {
    const char* name;
    SkillType type;
    int value; 
    bool harm;                  
    StatIndex affected_stat;    
} Skill;

typedef struct {
    const char* name;
    int stats[4];               
    int maxHealth;              
    bool guarding;
    Skill skills[6];            
} Character;

// Dichiarazioni delle funzioni
int RollDice(int type);
int CheckPriority(Character* ally, Character* enemy);
void ActionAttackPhysical(Character* attacker, Character* target, char* outMessage, int maxMsgLen);
void ActionSkill(Character* attacker, Character* target, Skill* skill, char* outMessage, int maxMsgLen);
void ActionUseObject(int itemIndex, Character* attacker, Character* target, char* outMessage, int maxMsgLen);
void ActionGuard(Character* character, char* outMessage, int maxMsgLen);
bool ActionEscape(Character* ally, Character* enemy, char* outMessage, int maxMsgLen);
bool ActionSpare(char* outMessage, int maxMsgLen);

extern Objects objectTable[];
extern int guard_stat;

#endif
