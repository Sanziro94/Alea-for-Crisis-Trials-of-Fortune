#include "ui.h"
#include "menustate.h"
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
static int itemTypesInBag = 5;
States gameState;
static int turn;
bool enemyTargetRandom = false;
bool fugaRiuscita = false;
bool graziaRicevuta = false;

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
                    characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_PIERCING, 5 , true, STAT_HP, 0};
                    break;
                case 4:
                    characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_VAMPIRISM, 5 , true, STAT_HP, 0};
                    break;
                case 5:
                    if (i == 0) characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_BUFF, 5 , false, STAT_ATK, 0};
                    if (i == 1) characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_BUFF, 5 , false, STAT_HP, 0};
                    if (i == 2) characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_BUFF, 2 , false, STAT_SPD, 0};
                    if (i == 3) characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_BUFF, 1 , false, STAT_DEF, 0};
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
void PlayerTurnLogic (Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {
    MenuStateTable[*menuState](mousePos, menuState, selectedCharacter, enemy, battleLog, maxLogLen);
}
static void EnemyTurnLogic(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {
    if (enemyTargetRandom) {
        EnemyTurnRandomTarget(enemy, battleLog, maxLogLen);
        enemyTargetRandom = false;
    } else 
        EnemyTurn(enemy, &characterButtons[*selectedCharacter].logic, battleLog, maxLogLen);
    gameState = ROUND_END_STATE;
}
static void EndTurnLogic(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {
    if (fugaRiuscita) *menuState = MENU_ESCAPED;
    if (graziaRicevuta) *menuState = MENU_MERCY;
    if (enemy->stats[STAT_HP] <= 0 && *menuState != MENU_VICTORY) *menuState = MENU_VICTORY;
    bool allDead = true;
    for (int i = 0; i < 4; i++) {
        if (characterButtons[i].logic.stats[STAT_HP] > 0 ) {
            allDead = false;
            break;
        }   
    }
    if (allDead && *menuState != MENU_DEFEAT) *menuState = MENU_DEFEAT;

    turn++;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
            if (characterButtons[i].skills[j].logic.cooldown > 0)
                characterButtons[i].skills[j].logic.cooldown--;
        } 
    }
    if (isPoisoned && poisonTurn > 0) {
        ApplyDamage(enemy, poisonDamage);
        poisonTurn--;
        if (poisonTurn == 0) isPoisoned = false;
    }
    if (*menuState != MENU_ESCAPED && *menuState != MENU_MERCY && *menuState != MENU_VICTORY && *menuState != MENU_DEFEAT)
        gameState = PLAYER_TURN_STATE;
}
void (*GameStateTable[])(Vector2, int* , int* , Character* , char* , int)={
    PlayerTurnLogic,
    EnemyTurnLogic,
    EndTurnLogic
};
void UpdateMenuLogic(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen) {

    // Isolated Clash state handling (state 8)
    if (*menuState == MENU_CLASH) {
        bool clashFinished = ActionClash(&characterButtons[*selectedCharacter].logic, enemy, battleLog, maxLogLen);
        if (clashFinished) *menuState = MENU_MAIN; 
        return;
    }
    GameStateTable[gameState](mousePos, menuState, selectedCharacter, enemy, battleLog, maxLogLen);
}
static void DrawMenuButtons(void){
    DrawRectangle(0, 500, 800, 100, DARKGRAY);
    for (int i = 0; i < 4; i++) {
        DrawRectangleRec(mainButtons[i].rect, LIGHTGRAY);
        DrawRectangleLinesEx(mainButtons[i].rect, 1, BLACK);
        DrawText(mainButtons[i].name, mainButtons[i].rect.x + 40, mainButtons[i].rect.y + 30, 20, BLACK);
    }
}
static void DrawCharacterButtons(Color baseColor, Color textColor) {
    DrawRectangle(0, 500, 800, 100, DARKGRAY);
    for (int i = 0; i < 4; i++) {
        DrawRectangleRec(characterButtons[i].rect, baseColor);
        DrawRectangleLinesEx(characterButtons[i].rect, 1, textColor);
        DrawText(characterButtons[i].name, characterButtons[i].rect.x + 20, characterButtons[i].rect.y + 30, 20, textColor);
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
    for(int i = 0; i < 4; i++) DrawText(TextFormat("%s HP: %d", characterButtons[i].name, characterButtons[i].logic.stats[STAT_HP]), 50 + i*180, 300, 18, GREEN);

    switch (gameState) {
        case PLAYER_TURN_STATE:
            switch (menuState) {
                case MENU_MAIN:
                    DrawMenuButtons();
                    break;  
                case MENU_SELECT_CHAR_ATTACK:
                    DrawCharacterButtons(MAROON, WHITE);
                    break;
                case MENU_SELECT_SKILL:
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
                case MENU_SELECT_CHAR_GUARD:
                    DrawCharacterButtons(SKYBLUE, BLACK);
                    break;
                case MENU_SELECT_CHAR_BAG:
                    DrawCharacterButtons(GOLD, BLACK);
                    break;
                case MENU_BAG_ITEMS:
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
                case MENU_ESCAPE_SPARE: 
                    DrawRectangle(250, 200, 300, 220, DARKGRAY);
                    DrawRectangleLines(250, 200, 300, 220, WHITE);
            
                    DrawRectangle(350, 240, 100, 40, LIGHTGRAY);
                    DrawText("Escape", 360, 250, 18, BLACK);

                    DrawRectangle(350, 310, 100, 40, LIGHTGRAY);
                    DrawText("Spare", 365, 320, 18, BLACK);
                    break;
                case MENU_CLASH:
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
            DrawMenuButtons();
            break;
        case ROUND_END_STATE:
            DrawMenuButtons();
            if (menuState == MENU_ESCAPED) {
                DrawRectangle(0, 0, 800, 600, BLACK);
                DrawText("ESCAPED!", 270, 250, 32, SKYBLUE);
            }
            if (menuState == MENU_MERCY) {
                DrawRectangle(0, 0, 800, 600, BLACK);
                DrawText("MERCY GIVEN!", 270, 250, 32, SKYBLUE);
            }
            if (menuState == MENU_VICTORY) {
                DrawRectangle(0, 0, 800, 600, BLACK);
                DrawText("VICTORY!", 270, 250, 32, GOLD);
            }
            if (menuState == MENU_DEFEAT) {
                DrawRectangle(0, 0, 800, 600, BLACK);
                DrawText("DEFEAT...", 270, 250, 32, RED);
            }
            break;
    }  
}