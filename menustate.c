#include "menustate.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void MenuMain(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {
    if (CheckCollisionPointRec(mousePos, mainButtons[0].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = MENU_SELECT_CHAR_ATTACK;
    if (CheckCollisionPointRec(mousePos, mainButtons[1].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = MENU_SELECT_CHAR_GUARD;
    if (CheckCollisionPointRec(mousePos, mainButtons[2].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = MENU_SELECT_CHAR_BAG;
    if (CheckCollisionPointRec(mousePos, mainButtons[3].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = MENU_ESCAPE_SPARE;
}
void MenuFight(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {
    for (int i = 0; i < 4; i++) {
        if (characterButtons[i].logic.stats[STAT_HP] > 0) {
            if (CheckCollisionPointRec(mousePos, characterButtons[i].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            *selectedCharacter = i;
            *menuState = MENU_SELECT_SKILL;
            }
        }
    }
}
void MenuSkills(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {
    for (int j = 0; j < 6; j++)  {  
        if (characterButtons[*selectedCharacter].skills[j].logic.cooldown == 0) {
            if (CheckCollisionPointRec(mousePos, characterButtons[*selectedCharacter].skills[j].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

                if (j > 2) characterButtons[*selectedCharacter].skills[j].logic.cooldown = 4; 

                if (characterButtons[*selectedCharacter].skills[j].logic.harm == true) {
                    if (rand() % 100 < 15 && j < 3 ) {
                        *menuState = MENU_CLASH;
                        return;
                    }
                    int priority = CheckPriority(&characterButtons[*selectedCharacter].logic, enemy);
                    if (priority == 1 || (priority == 2 && rand() % 2 == 0)) {
                        snprintf(battleLog, maxLogLen, "[INTERCEPT] %s is faster and moves first!", enemy->name);
                        EnemyTurn(enemy, &characterButtons[*selectedCharacter].logic, battleLog, maxLogLen);
                        if (characterButtons[*selectedCharacter].logic.stats[STAT_HP] > 0) {
                            char playerLog[256];
                            ActionSkill(&characterButtons[*selectedCharacter].logic, enemy, &characterButtons[*selectedCharacter].skills[j].logic, playerLog, sizeof(playerLog));
                            strncat(battleLog, "\n", maxLogLen - strlen(battleLog) - 1);
                            strncat(battleLog, playerLog, maxLogLen - strlen(battleLog) - 1);
                        } else {
                            strncat(battleLog, "\n...but the hero was knocked out before acting!", maxLogLen - strlen(battleLog) - 1);
                        }
                        gameState = ROUND_END_STATE;
                    } 
                    else {
                        ActionSkill(&characterButtons[*selectedCharacter].logic, enemy, &characterButtons[*selectedCharacter].skills[j].logic, battleLog, maxLogLen);
                        enemyTargetRandom = true; 
                        gameState = ENEMY_TURN_STATE;
                    }
                } 
                else {
                    ActionSkill(&characterButtons[*selectedCharacter].logic, enemy, &characterButtons[*selectedCharacter].skills[j].logic, battleLog, maxLogLen);
                    enemyTargetRandom = true; 
                    gameState = ENEMY_TURN_STATE;
                }
                *menuState = MENU_MAIN; 
                break;          
            }
        }
    } 
}
void MenuGuard(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {
    for (int i = 0; i < 4; i++) {
        if (characterButtons[i].logic.stats[STAT_HP] > 0) {
            if (CheckCollisionPointRec(mousePos, characterButtons[i].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ActionGuard(&characterButtons[i].logic, battleLog, maxLogLen);
                enemyTargetRandom = true; 
                gameState = ENEMY_TURN_STATE;
                *menuState = MENU_MAIN;
            }
        }
    }
}
void MenuBag(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {
    for (int i = 0; i < 4; i++) {
        if (characterButtons[i].logic.stats[STAT_HP] > 0) {
            if (CheckCollisionPointRec(mousePos, characterButtons[i].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                *selectedCharacter = i;
                *menuState = MENU_BAG_ITEMS;
            }
        }
    }
}
void MenuItems(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen){
    for (int i = 0; i < 5; i++) {
        if (bag[i].quantity > 0) {
            if (CheckCollisionPointRec(mousePos, bag[i].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ActionUseObject(bag[i].itemId, &characterButtons[*selectedCharacter].logic, enemy, battleLog, maxLogLen);
                bag[i].quantity--;
                enemyTargetRandom = true;
                gameState = ENEMY_TURN_STATE;
                *menuState = MENU_MAIN;
            }
        }
    }
    Rectangle backRect = { 40 + (5 % 3) * 240, 420 + (5 / 3) * 80, 220, 60 };
    if (CheckCollisionPointRec(mousePos, backRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
        *menuState = MENU_MAIN;  
}
void MenuEscape(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {
    int leaderIdx = 0;
    for (int i = 0; i < 4; i++) {
        if (characterButtons[i].logic.stats[STAT_HP] > 0) {
            leaderIdx = i;
            break;
        }
    }
    if (CheckCollisionPointRec(mousePos, (Rectangle){350, 240, 100, 40}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (ActionEscape(&characterButtons[leaderIdx].logic, enemy, battleLog, maxLogLen)) {
            fugaRiuscita = true;
            gameState = ROUND_END_STATE;
        } else {
            enemyTargetRandom = true;
            gameState = ENEMY_TURN_STATE;
            *menuState = MENU_MAIN;
            }
    }
    if (CheckCollisionPointRec(mousePos, (Rectangle){350, 310, 100, 40}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (ActionSpare(battleLog, maxLogLen)) {
            graziaRicevuta = true;
            gameState = ROUND_END_STATE;
        } else {
            enemyTargetRandom = true;
            gameState = ENEMY_TURN_STATE;
            *menuState = MENU_MAIN;
        }
    }
}
void (*MenuStateTable[])(Vector2, int* , int* , Character* , char* , int)={
    MenuMain,
    MenuFight,
    MenuSkills,
    MenuGuard,
    MenuBag,
    MenuItems,
    MenuEscape
};