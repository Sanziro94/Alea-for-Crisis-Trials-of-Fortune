#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdbool.h>
#include "raylib.h"

#define SEQUENCE_LENGTH 5

// Shared definitions
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
    int cooldown;
} Skill;

typedef struct {
    const char* name;
    int stats[4];               
    int maxHealth;              
    bool guarding;
    Skill skills[6];            
} Character;

// Gameplay function declarations
int RollDice(int type);
int CheckPriority(Character* ally, Character* enemy);
void ActionAttackPhysical(Character* attacker, Character* target, char* outMessage, int maxMsgLen);
void ActionSkill(Character* attacker, Character* target, Skill* skill, char* outMessage, int maxMsgLen);
void ActionUseObject(int itemIndex, Character* attacker, Character* target, char* outMessage, int maxMsgLen);
void ActionGuard(Character* character, char* outMessage, int maxMsgLen);
bool ActionEscape(Character* ally, Character* enemy, char* outMessage, int maxMsgLen);
bool ActionSpare(char* outMessage, int maxMsgLen);

// Functions for dynamic gameplay and Clash
bool ActionClash(Character* attacker, Character* target, char* outMessage, int maxMsgLen);
void EnemyTurn(Character* enemy, Character* target, char* outMessage, int maxMsgLen);

void EnemyTurnRandomTarget(Character* enemy, char* battleLog, int maxLogLen);

extern Objects objectTable[];
extern int guard_stat;
extern int poisonTurn;
extern int poisonDamage;
extern bool isPoisoned;


// Global Clash state shared with the UI
extern int currentStep;
extern float timeRemaining;
extern bool clashInitialized;
extern int targetSequence[SEQUENCE_LENGTH];
extern const char* sequenceNames[SEQUENCE_LENGTH];

#endif
