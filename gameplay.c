#include "gameplay.h" 
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "ui.h"

Objects objectTable[] = {
    {"Life Orb", HEAL, 20},
    {"Poison Jar", DAMAGE, 15},
    {"Spinach", BUFF_ATTACK, 5},
    {"Armor", BUFF_DEFENSE, 5},
    {"Speed Boost", BUFF_SPEED, 3}
};

Character enemy = {
        .name = "Boss Goblin",
        .maxHealth = 500,
        .stats = {500, 24, 5, 8}, // HP, ATK, DEF, SPD
        .guarding = false,
        .actionDone = false
    };

int guard_stat = 5;

int poisonTurn = 0;
int poisonDamage = 0;
bool isPoisoned = false;

int sottoTurni = 0;


int RollDice(int type) {
    int diceSides[] = {6, 12, 2, 100};
    int sides = (type >= 0 && type < 4) ? diceSides[type] : 6;
    return rand() % sides + 1;
}

int CheckPriority(Character* ally, Character* enemy) {
    if (ally->stats[STAT_SPD] > enemy->stats[STAT_SPD]) return 0; 
    if (ally->stats[STAT_SPD] < enemy->stats[STAT_SPD]) return 1; 
    return 2;                               
}
int ApplyDamage(Character* target, int dmg) {
    if (dmg < 1) dmg = 1;
    target->stats[STAT_HP] -= dmg;
    if (target->stats[STAT_HP] < 0) target->stats[STAT_HP] = 0;
    return dmg;
}
void ActionAttackPhysical(Character* attacker, Character* target, char* outMessage, int maxMsgLen) {
    int damage = attacker->stats[STAT_ATK] - target->stats[STAT_DEF];
    if (target->guarding) {
        damage -= guard_stat; 
        target->guarding = false; 
    }
    damage = ApplyDamage(target, damage);
    snprintf(outMessage, maxMsgLen, "%s attacks %s, dealing %d damage!", attacker->name, target->name, damage);
}

void ActionSkill(Character* attacker, Character* target, Skill* skill, char* outMessage, int maxMsgLen) {
    int dice = RollDice(0);
    int final_damage = 0;

    switch (skill->type) {
        case SKILL_NORMAL_DAMAGE:
            final_damage = (attacker->stats[STAT_ATK] + skill->value + dice) - target->stats[STAT_DEF];
            if (target->guarding) { final_damage -= guard_stat; target->guarding = false; }
            final_damage = ApplyDamage(target, final_damage);
            snprintf(outMessage, maxMsgLen, "%s uses %s! Dice: %d. Deals %d damage!", attacker->name, skill->name, dice, final_damage);
            break;

        case SKILL_PIERCING:
	        final_damage = (attacker->stats[STAT_ATK] + skill->value + dice);
            final_damage = ApplyDamage(target, final_damage);
            snprintf(outMessage, maxMsgLen, "%s uses %s (Piercing)! Ignores defense and deals %d damage!", attacker->name, skill->name, final_damage);
            break;

        case SKILL_VAMPIRISM:
            final_damage = (attacker->stats[STAT_ATK] + skill->value + dice) - target->stats[STAT_DEF];
            final_damage = ApplyDamage(target, final_damage);
            attacker->stats[STAT_HP] += final_damage / 2;
            if (attacker->stats[STAT_HP] > attacker->maxHealth) attacker->stats[STAT_HP] = attacker->maxHealth;
            snprintf(outMessage, maxMsgLen, "%s uses %s! Deals %d damage and heals for %d HP!", attacker->name, skill->name, final_damage, final_damage / 2);
            break;
        case SKILL_BUFF:
            attacker->stats[skill->affected_stat] += skill->value;
            const char* statNames[] = {"HP", "Attack", "Defense", "Speed"};
            snprintf(outMessage, maxMsgLen, "%s uses %s! Raises %s by %d!", attacker->name, skill->name, statNames[skill->affected_stat], skill->value);
            break;
    }
}

void ActionUseObject(int itemIndex, Character* attacker, Character* target, char* outMessage, int maxMsgLen)
{
    Objects obj = objectTable[itemIndex];
    switch (obj.type)
    {
        case HEAL:
            attacker->stats[STAT_HP] += obj.value;
            if (attacker->stats[STAT_HP] > attacker->maxHealth) attacker->stats[STAT_HP] = attacker->maxHealth;
            snprintf(outMessage, maxMsgLen, "%s uses %s and recovers %d HP!", attacker->name, obj.name, obj.value);
            break;
        case DAMAGE:
            poisonTurn = 3;
            poisonDamage = obj.value;
            isPoisoned = true;
            snprintf(outMessage, maxMsgLen, "%s throws %s and poison %s for %d turns!", attacker->name, obj.name, target->name, poisonTurn);
            break;
        case BUFF_ATTACK:
            attacker->stats[STAT_ATK] += obj.value;
            snprintf(outMessage, maxMsgLen, "%s uses %s! Attack increased!", attacker->name, obj.name);
            break;
        case BUFF_DEFENSE:
            attacker->stats[STAT_DEF] += obj.value;
            snprintf(outMessage, maxMsgLen, "%s uses %s! Defense increased!", attacker->name, obj.name);
            break;
        case BUFF_SPEED:
            attacker->stats[STAT_SPD] += obj.value;
            snprintf(outMessage, maxMsgLen, "%s uses %s! Speed increased!", attacker->name, obj.name);
            break;
    }
}

void ActionGuard(Character* character, char* outMessage, int maxMsgLen)
{
    character->guarding = true;
    snprintf(outMessage, maxMsgLen, "%s takes a defensive stance!", character->name);
}

bool ActionEscape(Character* ally, Character* enemy, char* outMessage, int maxMsgLen)
{
    int escape_chance = RollDice(3); 
    if (ally->stats[STAT_SPD] >= enemy->stats[STAT_SPD] && escape_chance > 30) return true;
        else if (escape_chance > 70) return true;
    snprintf(outMessage, maxMsgLen, "Escape failed! The enemy blocks your path!");
    return false;
}

bool ActionSpare(char* outMessage, int maxMsgLen)
{
    int dice = RollDice(0);
    if (dice >= 5) return true;
    snprintf(outMessage, maxMsgLen, "The enemy rejects your mercy!");
    return false;
}

// Global Clash variables
int currentStep = 0;
float timeRemaining = 3.0f;
bool clashInitialized = false;
int targetSequence[SEQUENCE_LENGTH] = { 0 };
const char* sequenceNames[SEQUENCE_LENGTH] = { NULL };

// Random key sequence generator for the QTE
static void GenerateClashSequence(void) {
    int keys[4] = { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };
    const char* names[4] = { "UP", "DOWN", "LEFT", "RIGHT" };

    for (int i = 0; i < SEQUENCE_LENGTH - 1; i++) {
        int r = rand() % 4;
        targetSequence[i] = keys[r];
        sequenceNames[i] = names[r];
    }
    targetSequence[SEQUENCE_LENGTH - 1] = KEY_SPACE;
    sequenceNames[SEQUENCE_LENGTH - 1] = "SPACE";
}

bool ActionClash(Character* attacker, Character* target, char* outMessage, int maxMsgLen)
{
    if (!clashInitialized) {
        currentStep = 0;
        timeRemaining = 2.5f + (attacker->stats[STAT_SPD] * 0.1f); 
        GenerateClashSequence();
        clashInitialized = true;
        snprintf(outMessage, maxMsgLen, "CLASH! Press the key sequence quickly!");
        return false; 
    }

    timeRemaining -= GetFrameTime();
    if (timeRemaining <= 0.0f) {
        snprintf(outMessage, maxMsgLen, "Time is up! %s wins the clash!", target->name);
        int dmg = (target->stats[STAT_ATK] * 2) - attacker->stats[STAT_DEF];
        dmg = ApplyDamage(attacker, dmg);
        clashInitialized = false;
        return true;
    }

    int expectedKey = targetSequence[currentStep];

    if (IsKeyPressed(expectedKey)) {
        currentStep++;
        if (currentStep >= SEQUENCE_LENGTH) {
            snprintf(outMessage, maxMsgLen, "%s wins the clash with a devastating blow!", attacker->name);
            int dmg = (attacker->stats[STAT_ATK] * 2) - target->stats[STAT_DEF];
            dmg = ApplyDamage(target, dmg);
            clashInitialized = false; 
            return true;
        }
    } 
    else {
        int pressedKey = GetKeyPressed();
        if (pressedKey != 0 && pressedKey != expectedKey) 
            timeRemaining -= 0.4f;   
    }
    return false; 
}
// Enemy AI logic
void EnemyTurn(Character* enemy, Character* target, char* outMessage, int maxMsgLen) {
    if (enemy->stats[STAT_HP] <= 0) return;

    int decision = rand() % 100;
    char enemyAction[128] = "";

    if (enemy->stats[STAT_HP] < (enemy->maxHealth * 0.35f) && decision < 50) 
        ActionGuard(enemy, enemyAction, sizeof(enemyAction));
    else if (decision > 75) {
        int dmg = (enemy->stats[STAT_ATK] * 1.5f) - target->stats[STAT_DEF];
        dmg = ApplyDamage(target, dmg);
        snprintf(enemyAction, sizeof(enemyAction), "%s unleashes a devastating heavy attack!", enemy->name);
    } 
    else 
        ActionAttackPhysical(enemy, target, enemyAction, sizeof(enemyAction));
    
    char tempLog[256];
    snprintf(tempLog, sizeof(tempLog), "%s", outMessage);
    snprintf(outMessage, maxMsgLen, "%s\n[ENEMY TURN]: %s", tempLog, enemyAction);
}
// The enemy attacks a random living hero
void EnemyTurnRandomTarget(Character* enemy, char* battleLog, int maxLogLen) {
    int aliveHeroes[4];
    int aliveCount = 0;
    for (int i = 0; i < 4; i++) {
        if (characterButtons[i].logic.stats[STAT_HP] > 0) 
            aliveHeroes[aliveCount++] = i;
    }
    if (aliveCount > 0) {
        int targetIdx = aliveHeroes[rand() % aliveCount];
        EnemyTurn(enemy, &characterButtons[targetIdx].logic, battleLog, maxLogLen);
    }
}