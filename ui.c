#include "ui.h"
#include "menustate.h"
#include "menudraw.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct CharacterUI characterButtons[4] = {
    {"Mercenary", {16 + 0*196, 510, 180, 80}, {{"Normal Skill M1"}, {"Normal Skill M2"}, {"Normal Skill M3"}, {"Piercing Skill M4"}, {"Vampirism Skill M5"}, {"Buff Skill M6"}}},
    {"Elf",       {16 + 1*196, 510, 180, 80}, {{"Normal Skill E1"}, {"Normal Skill E2"}, {"Normal Skill E3"}, {"Piercing Skill E4"}, {"Vampirism Skill E5"}, {"Buff Skill E6"}}},
    {"Assassin",  {16 + 2*196, 510, 180, 80}, {{"Normal Skill A1"}, {"Normal Skill A2"}, {"Normal Skill A3"}, {"Piercing Skill A4"}, {"Vampirism Skill A5"}, {"Buff Skill A6"}}},
    {"Cyborg",    {16 + 3*196, 510, 180, 80}, {{"Normal Skill C1"}, {"Normal Skill C2"}, {"Normal Skill C3"}, {"Piercing Skill C4"}, {"Vampirism Skill C5"}, {"Buff Skill C6"}}}
};
struct Buttons actionButtons[4] = {
    {"Fight",  {16 + 0*196, 510, 180, 80}},
    {"Guard",  {16 + 1*196, 510, 180, 80}},
    {"Bag",    {16 + 2*196, 510, 180, 80}}, 
    {"X", {745, 15, 40, 40}}  
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
    characterButtons[0].logic = (Character){"Mercenary", {100, 12, 8, 5}, 100, false, false};
    // Elf
    characterButtons[1].logic = (Character){"Elf", {80, 10, 5, 12}, 80, false, false};
    // Assassin
    characterButtons[2].logic = (Character){"Assassin", {75, 15, 4, 15}, 75, false, false};
    // Cyborg
    characterButtons[3].logic = (Character){"Cyborg", {120, 14, 10, 4}, 120, false, false};

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
    if (*menuState >= MENU_CHARACTERS && *menuState <= MENU_DEFEAT)
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
    
    if (isPoisoned && poisonTurn > 0) {
        ApplyDamage(enemy, poisonDamage);
        poisonTurn--;
        if (poisonTurn == 0) isPoisoned = false;
    }
    if (enemy->stats[STAT_HP] <= 0 && *menuState != MENU_VICTORY) *menuState = MENU_VICTORY;
    bool allDead = true;
    int alivePlayers = 0;
    for (int i = 0; i < 4; i++) {
        if (characterButtons[i].logic.stats[STAT_HP] > 0 ) {
            allDead = false;
            alivePlayers++;
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
    int deadPlayers = 4 - alivePlayers;
    int currentMaxSottoTurni = maxSottoTurni - deadPlayers;
    if (sottoTurni >= currentMaxSottoTurni) {
        for (int i = 0; i < 4; i++) {
            characterButtons[i].logic.actionDone = false;
        } 
        alivePlayers = 0;
        sottoTurni = 0;
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
        if (clashFinished) {
            gameState = ROUND_END_STATE;
            *menuState = MENU_CHARACTERS; 
        }
        return;
    }
    GameStateTable[gameState](mousePos, menuState, selectedCharacter, enemy, battleLog, maxLogLen);
}
static void DrawPlayerTurn(int menuState, int selectedCharacter) {
    if (menuState >= MENU_CHARACTERS && menuState <= MENU_DEFEAT)
        MenuDrawTable[menuState](selectedCharacter);
}
static void DrawEnemyTurn(int menuState, int selectedCharacter) {
    DrawCharacterButtons(selectedCharacter);
}
static void DrawTurnEnd(int menuState, int selectedCharacter) {
    DrawCharacterButtons(selectedCharacter);
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
}
void (*GameDrawTable[])(int, int)={
    DrawPlayerTurn,
    DrawEnemyTurn,
    DrawTurnEnd
};
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

    GameDrawTable[gameState](menuState, selectedCharacter);
}