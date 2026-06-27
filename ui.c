#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Buttons mainButtons[4] = {
    {"Fight",  {16 + 0*196, 510, 180, 80}},
    {"Guard",  {16 + 1*196, 510, 180, 80}},
    {"Bag",    {16 + 2*196, 510, 180, 80}}, 
    {"Escape", {16 + 3*196, 510, 180, 80}}  
};

struct CharacterUI characterButtons[4] = {
    {"Mercenary", {16 + 0*196, 510, 180, 80}, {{"Normal Skill M1"}, {"Normal Skill M2"}, {"Normal Skill M3"}, {"Piercing Skill M4"}, {"Vampirism Skill M5"}, {"Buff Skill M6"}}},
    {"Elf",       {16 + 1*196, 510, 180, 80}, {{"Normal Skill E1"}, {"Normal Skill E2"}, {"Normal Skill E3"}, {"Piercing Skill E4"}, {"Vampirism Skill E5"}, {"Buff Skill E6"}}},
    {"Assassin",  {16 + 2*196, 510, 180, 80}, {{"Normal Skill A1"}, {"Normal Skill A2"}, {"Normal Skill A3"}, {"Piercing Skill A4"}, {"Vampirism Skill A5"}, {"Buff Skill A6"}}},
    {"Cyborg",    {16 + 3*196, 510, 180, 80}, {{"Normal Skill C1"}, {"Normal Skill C2"}, {"Normal Skill C3"}, {"Piercing Skill C4"}, {"Vampirism Skill C5"}, {"Buff Skill C6"}}}
};

struct ObjectUI bag[5];
int itemTypesInBag = 5;
States gameState;
int turn;

void InitGameData(void) {
    // Mercenary
    characterButtons[0].logic = (Character){"Mercenary", {100, 12, 8, 5}, 100, false};
    // Elf
    characterButtons[1].logic = (Character){"Elf", {80, 10, 5, 12}, 80, false};
    // Assassin
    characterButtons[2].logic = (Character){"Assassin", {75, 15, 4, 15}, 75, false};
    // Cyborg
    characterButtons[3].logic = (Character){"Cyborg", {120, 14, 10, 4}, 120, false};

    // Assign rectangle positions for skills
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
            characterButtons[i].skills[j].rect.x = 40 + (j % 3) * 240;
            characterButtons[i].skills[j].rect.y = 420 + (j / 3) * 80;
            characterButtons[i].skills[j].rect.width = 220;
            characterButtons[i].skills[j].rect.height = 60;
            switch (j) {
                case 3:
                    characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_PIERCING, 5 + j, true, STAT_HP, 0};
                    break;
                case 4:
                    characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_VAMPIRISM, 5 + j, true, STAT_HP, 0};
                    break;
                case 5:
                    if (i == 0) characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_BUFF, 5 + j, false, STAT_ATK, 0};
                    if (i == 1) characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_BUFF, 5 + j, false, STAT_HP, 0};
                    if (i == 2) characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_BUFF, 5 + j, false, STAT_SPD, 0};
                    if (i == 3) characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_BUFF, 5 + j, false, STAT_DEF, 0};
                    break;
                default:
                    characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_NORMAL_DAMAGE, 5 + j, true, STAT_HP, 0};
                    break;
            }
        }
    }
    // Assign rectangle positions for items
    for (int i = 0; i < 5; i++) {
        bag[i].rect.x = 40 + (i % 3) * 240; 
        bag[i].rect.y = 420 + (i / 3) * 80; 
        bag[i].rect.width = 220;
        bag[i].rect.height = 60;
        bag[i].itemId = i; 
        snprintf(bag[i].name, sizeof(bag[i].name), "%s", objectTable[i].name);
        if (i == 0) bag[i].quantity = 2; 
        else bag[i].quantity = 1;
            
    }
}

void UpdateMenuLogic(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {

    static bool enemyTargetRandom = false;
    static bool fugaRiuscita = false;
    static bool graziaRicevuta = false;
    // Isolated Clash state handling (state 8)
    if (*menuState == 8) {
        bool clashFinished = ActionClash(&characterButtons[*selectedCharacter].logic, enemy, battleLog, maxLogLen);
        if (clashFinished) {
            *menuState = 0; 
        }
        return;
    }

    switch (gameState) {
        case PLAYER_TURN_STATE:
            switch (*menuState) {
                case 0:
                    if (CheckCollisionPointRec(mousePos, mainButtons[0].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = 1;
                    if (CheckCollisionPointRec(mousePos, mainButtons[1].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = 3;
                    if (CheckCollisionPointRec(mousePos, mainButtons[2].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = 4;
                    if (CheckCollisionPointRec(mousePos, mainButtons[3].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = 6;
                    break;
                case 1:
                    for (int i = 0; i < 4; i++) {
                        if (characterButtons[i].logic.stats[STAT_HP] > 0) {
                            if (CheckCollisionPointRec(mousePos, characterButtons[i].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            *selectedCharacter = i;
                            *menuState = 2;
                            }
                        }
                    }
                    break;
                case 2:
                    for (int j = 0; j < 6; j++)  {  
                        if (characterButtons[*selectedCharacter].skills[j].logic.cooldown == 0) {
                            if (CheckCollisionPointRec(mousePos, characterButtons[*selectedCharacter].skills[j].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

                                if (j > 2) characterButtons[*selectedCharacter].skills[j].logic.cooldown = 4; 

                                if (characterButtons[*selectedCharacter].skills[j].logic.harm == true) {
                                    if (rand() % 100 < 15 && j < 3 ) {
                                        *menuState = 8;
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
                                *menuState = 0; 
                                break;          
                            }
                        }
                    } 
                    break;
                case 3:
                    for (int i = 0; i < 4; i++) {
                        if (characterButtons[i].logic.stats[STAT_HP] > 0) {
                            if (CheckCollisionPointRec(mousePos, characterButtons[i].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                ActionGuard(&characterButtons[i].logic, battleLog, maxLogLen);
                                enemyTargetRandom = true; 
                                gameState = ENEMY_TURN_STATE;
                                *menuState = 0;
                            }
                        }
                    }
                    break;
                case 4: 
                    for (int i = 0; i < 4; i++) {
                        if (characterButtons[i].logic.stats[STAT_HP] > 0) {
                            if (CheckCollisionPointRec(mousePos, characterButtons[i].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                *selectedCharacter = i;
                                *menuState = 5;
                            }
                        }
                    }
                    break;
                case 5:
                    for (int i = 0; i < 5; i++) {
                        if (bag[i].quantity > 0) {
                            if (CheckCollisionPointRec(mousePos, bag[i].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                ActionUseObject(bag[i].itemId, &characterButtons[*selectedCharacter].logic, enemy, battleLog, maxLogLen);
                                bag[i].quantity--;
                                enemyTargetRandom = true;
                                gameState = ENEMY_TURN_STATE;
                                *menuState = 0;
                            }
                        }
                    }
                    Rectangle backRect = { 40 + (5 % 3) * 240, 420 + (5 / 3) * 80, 220, 60 };
                    if (CheckCollisionPointRec(mousePos, backRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
                        *menuState = 0;       
                    break;
                case 6:      
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
                            *menuState = 0;
                            }
                    }
                    if (CheckCollisionPointRec(mousePos, (Rectangle){350, 310, 100, 40}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        if (ActionSpare(battleLog, maxLogLen)) {
                            graziaRicevuta = true;
                            gameState = ROUND_END_STATE;
                        } else {
                            enemyTargetRandom = true;
                            gameState = ENEMY_TURN_STATE;
                            *menuState = 0;
                        }
                    }
                    break;
            }
            break;
        case ENEMY_TURN_STATE:
            if (enemyTargetRandom) {
                EnemyTurnRandomTarget(enemy, battleLog, maxLogLen);
                enemyTargetRandom = false;
            } else {
                EnemyTurn(enemy, &characterButtons[*selectedCharacter].logic, battleLog, maxLogLen);
            }
            gameState = ROUND_END_STATE;
            break;
        case ROUND_END_STATE:
            if (fugaRiuscita) *menuState = 7;
            if (graziaRicevuta) *menuState = 9;
            if (enemy->stats[STAT_HP] <= 0 && *menuState != 10) *menuState = 10;
            bool allDead = true;
            for (int i = 0; i < 4; i++) {
                if (characterButtons[i].logic.stats[STAT_HP] > 0 ) {
                    allDead = false;
                    break;
                }   
            }
            if (allDead && *menuState != 11) *menuState = 11;

            turn++;
            for (int i = 0; i < 4; i++) {
                for (int j = 3; j < 6; j++) {
                    if (characterButtons[i].skills[j].logic.cooldown > 0)
                        characterButtons[i].skills[j].logic.cooldown--;
                } 
            }
            if (isPoisoned && poisonTurn > 0) {
                enemy->stats[STAT_HP] -= poisonDamage;
                if (enemy->stats[STAT_HP] < 0) enemy->stats[STAT_HP] = 0;
                poisonTurn--;
                if (poisonTurn == 0) isPoisoned = false;
            }
            if (*menuState != 7 && *menuState != 9 && *menuState != 10 && *menuState != 11)
                gameState = PLAYER_TURN_STATE;
            break;    
    }  
}

void DrawMenuUI(int menuState, int selectedCharacter, const Character* enemy, const char* battleLog) {
    // Enemy info, always visible on screen
    DrawText(enemy->name, 50, 50, 22, RED);
    DrawText(TextFormat("HP: %d/%d", enemy->stats[STAT_HP], enemy->maxHealth), 50, 80, 20, LIGHTGRAY);

    // Battle log box
    DrawRectangle(50, 150, 700, 100, DARKGRAY);
    DrawRectangleLines(50, 150, 700, 100, WHITE);
    DrawText(battleLog, 65, 165, 16, WHITE);

    // Hero info
    for(int i = 0; i < 4; i++) {
        DrawText(TextFormat("%s HP: %d", characterButtons[i].name, characterButtons[i].logic.stats[STAT_HP]), 50 + i*180, 300, 18, GREEN);
    }

    switch (gameState) {
        case PLAYER_TURN_STATE:
            switch (menuState) {
                case 0:
                    DrawRectangle(0, 500, 800, 100, DARKGRAY);
                    for (int i = 0; i < 4; i++) {
                        DrawRectangleRec(mainButtons[i].rect, LIGHTGRAY);
                        DrawRectangleLinesEx(mainButtons[i].rect, 1, BLACK);
                        DrawText(mainButtons[i].name, mainButtons[i].rect.x + 40, mainButtons[i].rect.y + 30, 20, BLACK);
                    }
                    break;  
                case 1:
                    DrawRectangle(0, 500, 800, 100, DARKGRAY);
                    for (int i = 0; i < 4; i++) {
                        DrawRectangleRec(characterButtons[i].rect, MAROON);
                        DrawRectangleLinesEx(characterButtons[i].rect, 1, WHITE);
                        DrawText(characterButtons[i].name, characterButtons[i].rect.x + 20, characterButtons[i].rect.y + 30, 20, WHITE);
                    }
                    break;
                case 2:
                    DrawRectangle(0, 400, 800, 200, DARKGRAY);
                    for (int j = 0; j < 6; j++) {
                        if (characterButtons[selectedCharacter].skills[j].logic.cooldown == 0) {
                            Rectangle r = characterButtons[selectedCharacter].skills[j].rect;
                            DrawRectangleRec(r, MAROON);
                            DrawRectangleLinesEx(r, 1, WHITE);
                            DrawText(characterButtons[selectedCharacter].skills[j].name, r.x + 15, r.y + 20, 18, WHITE);
                        }
                        if (characterButtons[selectedCharacter].skills[j].logic.cooldown > 0) {
                            Rectangle r = characterButtons[selectedCharacter].skills[j].rect;
                            DrawRectangleRec(r, GRAY);
                            DrawRectangleLinesEx(r, 1, DARKGRAY);
                            DrawText("Recharging", r.x + 15, r.y + 20, 18, DARKGRAY);
                        }
                    }
                    break;
                case 3:
                    DrawRectangle(0, 500, 800, 100, DARKGRAY);
                    for (int i = 0; i < 4; i++) {
                        DrawRectangleRec(characterButtons[i].rect, SKYBLUE);
                        DrawText(characterButtons[i].name, characterButtons[i].rect.x + 20, characterButtons[i].rect.y + 30, 20, BLACK);
                    }
                    break;
                case 4:
                    DrawRectangle(0, 500, 800, 100, DARKGRAY);
                    for (int i = 0; i < 4; i++) {
                        DrawRectangleRec(characterButtons[i].rect, GOLD);
                        DrawText(characterButtons[i].name, characterButtons[i].rect.x + 20, characterButtons[i].rect.y + 30, 20, BLACK);
                    }
                    break;
                case 5:
                    DrawRectangle(0, 400, 800, 200, DARKGRAY);
                    for (int i = 0; i < 5; i++) {
                        if (bag[i].quantity > 0) {
                            DrawRectangleRec(bag[i].rect, LIGHTGRAY);
                            DrawRectangleLinesEx(bag[i].rect, 1, BLACK);
                            DrawText(TextFormat("%s (x%d)", bag[i].name, bag[i].quantity), bag[i].rect.x + 15, bag[i].rect.y + 20, 18, BLACK);
                        } else {
                            DrawRectangleRec(bag[i].rect, GRAY);
                            DrawRectangleLinesEx(bag[i].rect, 1, DARKGRAY);
                            DrawText("Empty", bag[i].rect.x + 15, bag[i].rect.y + 20, 18, DARKGRAY);
                        }
                    }
                    int backX = 40 + (5 % 3) * 240;
                    int backY = 420 + (5 / 3) * 80;
                    DrawRectangle(backX, backY, 220, 60, RED);
                    DrawRectangleLines(backX, backY, 220, 60, WHITE);
                    DrawText("BACK", backX + 80, backY + 20, 18, WHITE);  
                    break;
                case 6: 
                    DrawRectangle(250, 200, 300, 220, DARKGRAY);
                    DrawRectangleLines(250, 200, 300, 220, WHITE);
            
                    DrawRectangle(350, 240, 100, 40, LIGHTGRAY);
                    DrawText("Escape", 360, 250, 18, BLACK);

                    DrawRectangle(350, 310, 100, 40, LIGHTGRAY);
                    DrawText("Spare", 365, 320, 18, BLACK);
                    break;
                case 8:
                    DrawRectangle(100, 380, 600, 200, BLACK);
                    DrawRectangleLines(100, 380, 600, 200, RED);
                    DrawText("!! ATTACK CLASH !!", 295, 400, 24, ORANGE);
            
                    for (int i = 0; i < SEQUENCE_LENGTH; i++) {
                        Color stepColor = WHITE;
                        if (i < currentStep) stepColor = GREEN;      
                        else if (i == currentStep) stepColor = YELLOW; 
                
                        if (sequenceNames[i] != NULL) {
                            DrawText(sequenceNames[i], 160 + (i * 100), 450, 22, stepColor);
                        }
                    }
            
                    float maxClashTime = 2.5f + (characterButtons[selectedCharacter].logic.stats[STAT_SPD] * 0.1f);
                    DrawRectangle(200, 520, (int)(400 * (timeRemaining / maxClashTime)), 15, RED);
                    DrawRectangleLines(200, 520, 400, 15, WHITE);
                    break;
            }
            break;
        case ENEMY_TURN_STATE:
            DrawRectangle(0, 500, 800, 100, DARKGRAY);
            for (int i = 0; i < 4; i++) {
                DrawRectangleRec(mainButtons[i].rect, LIGHTGRAY);
                DrawRectangleLinesEx(mainButtons[i].rect, 1, BLACK);
                DrawText(mainButtons[i].name, mainButtons[i].rect.x + 40, mainButtons[i].rect.y + 30, 20, BLACK);
            }
            break;
        case ROUND_END_STATE:
            DrawRectangle(0, 500, 800, 100, DARKGRAY);
            for (int i = 0; i < 4; i++) {
                DrawRectangleRec(mainButtons[i].rect, LIGHTGRAY);
                DrawRectangleLinesEx(mainButtons[i].rect, 1, BLACK);
                DrawText(mainButtons[i].name, mainButtons[i].rect.x + 40, mainButtons[i].rect.y + 30, 20, BLACK);
            }
            if (menuState == 7) {
                DrawRectangle(0, 0, 800, 600, BLACK);
                DrawText("ESCAPED!", 270, 250, 32, SKYBLUE);
            }
            if (menuState == 9) {
                DrawRectangle(0, 0, 800, 600, BLACK);
                DrawText("MERCY GIVEN!", 270, 250, 32, SKYBLUE);
            }
            if (menuState == 10) {
                DrawRectangle(0, 0, 800, 600, BLACK);
                DrawText("VICTORY!", 270, 250, 32, GOLD);
            }
            if (menuState == 11) {
                DrawRectangle(0, 0, 800, 600, BLACK);
                DrawText("DEFEAT...", 270, 250, 32, RED);
            }
            break;
    }  
}
