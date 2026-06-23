#include "gameplay.h" 
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "ui_shared.h"

Objects objectTable[] = {
    {"Life_orb", HEAL, 20},
    {"Poison_jar", DAMAGE, 15},
    {"Speed_boost", BUFF_SPEED, 3},
    {"Spinach", BUFF_ATTACK, 5},
    {"Armor", BUFF_DEFENSE, 5}
};

int guard_stat = 5; 

int RollDice(int type)
{
    switch(type)
    {
        case 0:  return rand() % 6 + 1;
        case 1:  return rand() % 12 + 1;
        case 2:  return rand() % 2 + 1;
        case 3:  return rand() % 100 + 1;
        default: return rand() % 6 + 1;
    }
}

int CheckPriority(Character* ally, Character* enemy)
{
    if (ally->stats[STAT_SPD] > enemy->stats[STAT_SPD]) return 0; 
    if (ally->stats[STAT_SPD] < enemy->stats[STAT_SPD]) return 1; 
    return 2;                               
}

void ActionAttackPhysical(Character* attacker, Character* target, char* outMessage, int maxMsgLen)
{
    int base_damage = attacker->stats[STAT_ATK];
    int defense = target->stats[STAT_DEF];
    int final_damage = base_damage - defense;

    if (target->guarding) {
        final_damage -= guard_stat; 
        target->guarding = false; 
    }

    if (final_damage <= 0) final_damage = 1;

    target->stats[STAT_HP] -= final_damage;
    if (target->stats[STAT_HP] < 0) target->stats[STAT_HP] = 0;

    snprintf(outMessage, maxMsgLen, "%s attacca %s infliggendo %d danni!", attacker->name, target->name, final_damage);
}

void ActionSkill(Character* attacker, Character* target, Skill* skill, char* outMessage, int maxMsgLen)
{
    int dice = RollDice(0);
    int final_damage = 0;

    switch (skill->type)
    {
        case SKILL_NORMAL_DAMAGE:
            final_damage = (attacker->stats[STAT_ATK] + skill->value + dice) - target->stats[STAT_DEF];
            if (target->guarding) { final_damage -= guard_stat; target->guarding = false; }
            if (final_damage <= 0) final_damage = 1;
            target->stats[STAT_HP] -= final_damage;
            snprintf(outMessage, maxMsgLen, "%s usa %s! Dado: %d. Infligge %d danni!", attacker->name, skill->name, dice, final_damage);
            break;

        case SKILL_PIERCING:
            final_damage = (attacker->stats[STAT_ATK] + skill->value + dice);
            if (final_damage <= 0) final_damage = 1;
            target->stats[STAT_HP] -= final_damage;
            snprintf(outMessage, maxMsgLen, "%s usa %s (Perforante)! Ignora la difesa e fa %d danni!", attacker->name, skill->name, final_damage);
            break;

        case SKILL_VAMPIRISM:
            final_damage = (attacker->stats[STAT_ATK] + skill->value + dice) - target->stats[STAT_DEF];
            if (final_damage <= 0) final_damage = 1;
            target->stats[STAT_HP] -= final_damage;
            attacker->stats[STAT_HP] += final_damage / 2;
            if (attacker->stats[STAT_HP] > attacker->maxHealth) attacker->stats[STAT_HP] = attacker->maxHealth;
            snprintf(outMessage, maxMsgLen, "%s usa %s! Fa %d danni e si cura di %d!", attacker->name, skill->name, final_damage, final_damage / 2);
            break;

        case SKILL_BUFF:
            attacker->stats[skill->affected_stat] += skill->value;
            snprintf(outMessage, maxMsgLen, "%s usa %s! Aumenta la statistica di %d!", attacker->name, skill->name, skill->value);
            break;
    }
    if (target->stats[STAT_HP] < 0) target->stats[STAT_HP] = 0;
}

void ActionUseObject(int itemIndex, Character* attacker, Character* target, char* outMessage, int maxMsgLen)
{
    Objects obj = objectTable[itemIndex];
    switch (obj.type)
    {
        case HEAL:
            attacker->stats[STAT_HP] += obj.value;
            if (attacker->stats[STAT_HP] > attacker->maxHealth) attacker->stats[STAT_HP] = attacker->maxHealth;
            snprintf(outMessage, maxMsgLen, "%s usa %s e recupera %d HP!", attacker->name, obj.name, obj.value);
            break;
        case DAMAGE:
            target->stats[STAT_HP] -= obj.value;
            if (target->stats[STAT_HP] < 0) target->stats[STAT_HP] = 0;
            snprintf(outMessage, maxMsgLen, "%s lancia %s e infligge %d danni a %s!", attacker->name, obj.name, obj.value, target->name);
            break;
        case BUFF_ATTACK:
            attacker->stats[STAT_ATK] += obj.value;
            snprintf(outMessage, maxMsgLen, "%s usa %s! Attacco aumentato!", attacker->name, obj.name);
            break;
        case BUFF_DEFENSE:
            attacker->stats[STAT_DEF] += obj.value;
            snprintf(outMessage, maxMsgLen, "%s usa %s! Difesa aumentata!", attacker->name, obj.name);
            break;
        case BUFF_SPEED:
            attacker->stats[STAT_SPD] += obj.value;
            snprintf(outMessage, maxMsgLen, "%s usa %s! Velocità aumentata!", attacker->name, obj.name);
            break;
    }
}

void ActionGuard(Character* character, char* outMessage, int maxMsgLen)
{
    character->guarding = true;
    snprintf(outMessage, maxMsgLen, "%s si mette in posizione di guardia!", character->name);
}

bool ActionEscape(Character* ally, Character* enemy, char* outMessage, int maxMsgLen)
{
    int escape_chance = RollDice(3); 
    if (ally->stats[STAT_SPD] >= enemy->stats[STAT_SPD] && escape_chance > 30) {
        snprintf(outMessage, maxMsgLen, "Fuga riuscita con successo!");
        return true;
    } else if (escape_chance > 70) {
        snprintf(outMessage, maxMsgLen, "Fuga rocambolesca riuscita!");
        return true;
    }
    snprintf(outMessage, maxMsgLen, "Fuga fallita! Il nemico ti blocca la strada!");
    return false;
}

bool ActionSpare(char* outMessage, int maxMsgLen)
{
    int dice = RollDice(0);
    if (dice >= 5) {
        snprintf(outMessage, maxMsgLen, "Il nemico ha accettato la grazia! Vittoria!");
        return true;
    }
    snprintf(outMessage, maxMsgLen, "Il nemico rifiuta la pietà!");
    return false;
}

// Definizione delle variabili globali del Clash
int currentStep = 0;
float timeRemaining = 3.0f;
bool clashInitialized = false;
int targetSequence[SEQUENCE_LENGTH] = { 0 };
const char* sequenceNames[SEQUENCE_LENGTH] = { NULL };

// Generatore casuale della sequenza di tasti per il QTE
void GenerateClashSequence(void) {
    int keys[4] = { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };
    const char* names[4] = { "SU", "GIU", "SX", "DX" };

    for (int i = 0; i < SEQUENCE_LENGTH - 1; i++) {
        int r = rand() % 4;
        targetSequence[i] = keys[r];
        sequenceNames[i] = names[r];
    }
    targetSequence[SEQUENCE_LENGTH - 1] = KEY_SPACE;
    sequenceNames[SEQUENCE_LENGTH - 1] = "SPAZIO";
}

bool ActionClash(Character* attacker, Character* target, char* outMessage, int maxMsgLen)
{
    if (!clashInitialized) {
        currentStep = 0;
        timeRemaining = 2.5f + (attacker->stats[STAT_SPD] * 0.1f); 
        GenerateClashSequence();
        clashInitialized = true;
        snprintf(outMessage, maxMsgLen, "CLASH! Premi la sequenza di tasti rapidamente!");
        return false; 
    }

    timeRemaining -= GetFrameTime();
    if (timeRemaining <= 0.0f) {
        snprintf(outMessage, maxMsgLen, "Tempo scaduto! %s vince lo scontro d'impatto!", target->name);
        int dmg = (target->stats[STAT_ATK] * 2) - attacker->stats[STAT_DEF];
        if (dmg < 1) dmg = 1;
        attacker->stats[STAT_HP] -= dmg;
        if (attacker->stats[STAT_HP] < 0) attacker->stats[STAT_HP] = 0;
        clashInitialized = false;
        return true;
    }

    int expectedKey = targetSequence[currentStep];

    if (IsKeyPressed(expectedKey)) {
        currentStep++;
        if (currentStep >= SEQUENCE_LENGTH) {
            snprintf(outMessage, maxMsgLen, "%s vince il clash con un colpo devastante!", attacker->name);
            int dmg = (attacker->stats[STAT_ATK] * 2) - target->stats[STAT_DEF];
            if (dmg < 1) dmg = 1;
            target->stats[STAT_HP] -= dmg;
            if (target->stats[STAT_HP] < 0) target->stats[STAT_HP] = 0;
            clashInitialized = false; 
            return true;
        }
    } 
    else {
        int pressedKey = GetKeyPressed();
        if (pressedKey != 0 && pressedKey != expectedKey) {
            timeRemaining -= 0.4f; 
        }
    }

    return false; 
}

// Logica dell'Intelligenza Artificiale del Nemico
void EnemyTurn(Character* enemy, Character* target, char* outMessage, int maxMsgLen) {
    if (enemy->stats[STAT_HP] <= 0) return;

    int decision = rand() % 100;
    char enemyAction[128] = "";

    if (enemy->stats[STAT_HP] < (enemy->maxHealth * 0.35f) && decision < 50) {
        ActionGuard(enemy, enemyAction, sizeof(enemyAction));
    } 
    else if (decision > 75) {
        int dmg = (enemy->stats[STAT_ATK] * 1.5f) - target->stats[STAT_DEF];
        if (dmg < 1) dmg = 1;
        target->stats[STAT_HP] -= dmg;
        if (target->stats[STAT_HP] < 0) target->stats[STAT_HP] = 0;
        snprintf(enemyAction, sizeof(enemyAction), "%s scatena un attacco pesante devastante!", enemy->name);
    } 
    else {
        ActionAttackPhysical(enemy, target, enemyAction, sizeof(enemyAction));
    }

    char tempLog[256];
    strncpy(tempLog, outMessage, sizeof(tempLog));
    snprintf(outMessage, maxMsgLen, "%s\n[TURNO NEMICO]: %s", tempLog, enemyAction);
}
//Il Nemico attacca Un Eroe Random
void EnemyTurnRandomTarget(Character* enemy, char* battleLog, int maxLogLen) {
    int vivi[4];
    int conteggioVivi = 0;
    for (int i = 0; i < 4; i++) {
        if (characterButtons[i].logic.stats[STAT_HP] > 0) {
            vivi[conteggioVivi++] = i;
        }
    }
    if (conteggioVivi > 0) {
        int targetIdx = vivi[rand() % conteggioVivi];
        EnemyTurn(enemy, &characterButtons[targetIdx].logic, battleLog, maxLogLen);
    }
}
