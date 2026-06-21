#include "gameplay.h" 
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

Objects objectTable[] = {
    {"Life_orb", HEAL, 20},
    {"Poison_jar", DAMAGE, 15},
    {"Speed_boost", BUFF_SPEED, 3}
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
    int dodge = rand() % 100;

    if (dodge < target->stats[STAT_SPD] * 3)
    {
        snprintf(outMessage, maxMsgLen, "%s ha schivato l'attacco!", target->name);
        return;
    }

    // Inserito tipo di dado (0) per la funzione RollDice
    int dmg = attacker->stats[STAT_ATK] + RollDice(0) - target->stats[STAT_DEF];
    bool blocked = false;

    if (target->guarding)
    {
        dmg -= guard_stat;
        target->guarding = false;
        blocked = true;
    }

    bool critical = false;
    if (RollDice(0) == 6)
    {
        dmg *= 2;
        critical = true;
    }

    if (dmg < 1) dmg = 1;
    target->stats[STAT_HP] -= dmg;
    if (target->stats[STAT_HP] < 0) target->stats[STAT_HP] = 0;

    snprintf(outMessage, maxMsgLen, "%s%s attacca %s per %d danni! %s",
             critical ? "CRITICO! " : "", attacker->name, target->name, dmg, blocked ? "(Parato!)" : "");
}

void ActionSkill(Character* attacker, Character* target, Skill* skill, char* outMessage, int maxMsgLen)
{
    if (skill->harm)
    {
        int dodge = rand() % 80;
        if (dodge < target->stats[STAT_SPD] * 3)
        {
            snprintf(outMessage, maxMsgLen, "%s ha schivato la skill: %s!", target->name, skill->name);
            return;
        }
    }

    int dmg = 0;

    switch (skill->type)
    {
        case SKILL_NORMAL_DAMAGE:
            dmg = attacker->stats[STAT_ATK] + skill->value - target->stats[STAT_DEF];
            if (dmg < 1) dmg = 1;
            target->stats[STAT_HP] -= dmg;
            snprintf(outMessage, maxMsgLen, "%s lancia %s! Fa %d danni a %s.", attacker->name, skill->name, dmg, target->name);
            break;

        case SKILL_PIERCING:
            dmg = skill->value; 
            target->stats[STAT_HP] -= dmg;
            snprintf(outMessage, maxMsgLen, "%s usa %s! Ignora la difesa e infligge %d danni!", attacker->name, skill->name, dmg);
            break;

        case SKILL_VAMPIRISM:
            dmg = skill->value;
            target->stats[STAT_HP] -= dmg;
            
            attacker->stats[STAT_HP] += (dmg / 2);
            if (attacker->stats[STAT_HP] > attacker->maxHealth) attacker->stats[STAT_HP] = attacker->maxHealth;
            
            snprintf(outMessage, maxMsgLen, "%s usa %s! Fa %d danni e assorbe %d HP!", attacker->name, skill->name, dmg, dmg / 2);
            break;

        case SKILL_BUFF:
            // Mappa l'effetto usando direttamente la variabile d'istanza come indice dell'array
            attacker->stats[skill->affected_stat] += skill->value;
            
            if (skill->affected_stat == STAT_HP && attacker->stats[STAT_HP] > attacker->maxHealth) {
                attacker->stats[STAT_HP] = attacker->maxHealth;
            }
            
            snprintf(outMessage, maxMsgLen, "%s attiva %s! Incremento statistica di %d.", attacker->name, skill->name, skill->value);
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
            snprintf(outMessage, maxMsgLen, "%s usa %s e cura %d HP!", attacker->name, obj.name, obj.value);
            break;

        case DAMAGE:
            target->stats[STAT_HP] -= obj.value;
            if (target->stats[STAT_HP] < 0) target->stats[STAT_HP] = 0;
            snprintf(outMessage, maxMsgLen, "%s lancia %s! %s subisce %d danni!", attacker->name, obj.name, target->name, obj.value);
            break;

        case BUFF_ATTACK:
            attacker->stats[STAT_ATK] += obj.value;
            snprintf(outMessage, maxMsgLen, "Attacco di %s aumentato!", attacker->name);
            break;

        case BUFF_DEFENSE:
            attacker->stats[STAT_DEF] += obj.value;
            snprintf(outMessage, maxMsgLen, "Difesa di %s aumentata!", attacker->name);
            break;

        case BUFF_SPEED:
            attacker->stats[STAT_SPD] += obj.value;
            snprintf(outMessage, maxMsgLen, "Velocità di %s aumentata!", attacker->name);
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
    if (ally->stats[STAT_SPD] > enemy->stats[STAT_SPD])
    {
        snprintf(outMessage, maxMsgLen, "Fuga riuscita con successo!");
        return true;
    }
    snprintf(outMessage, maxMsgLen, "Fuga fallita!");
    return false;
}

bool ActionSpare(char* outMessage, int maxMsgLen)
{
    int dice = RollDice(0);
    if (dice == 1 || dice == 3 || dice == 5)
    {
        snprintf(outMessage, maxMsgLen, "Il nemico ha accettato la grazia! Vittoria!");
        return true;
    }
    snprintf(outMessage, maxMsgLen, "Il nemico rifiuta la pietà!");
    return false;
}