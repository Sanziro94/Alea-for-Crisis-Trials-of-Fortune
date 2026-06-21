#include "ui_shared.h"
#include <stdio.h>

struct Buttons mainButtons[4] = {
    {"Lotta",   16 + 0*196, 510, 180, 80},
    {"Guardia", 16 + 1*196, 510, 180, 80},
    {"Borsa",   16 + 2*196, 510, 180, 80}, 
    {"Fuga",    16 + 3*196, 510, 180, 80}  
};

struct CharacterUI characterButtons[4] = {
    {"Mercenary", 16 + 0*196, 510, 180, 80, {{"Skill M1"}, {"Skill M2"}, {"Skill M3"}, {"Skill M4"}, {"Skill M5"}, {"Skill M6"}}},
    {"Elf",       16 + 1*196, 510, 180, 80, {{"Skill E1"}, {"Skill E2"}, {"Skill E3"}, {"Skill E4"}, {"Skill E5"}, {"Skill E6"}}},
    {"Assassin",  16 + 2*196, 510, 180, 80, {{"Skill A1"}, {"Skill A2"}, {"Skill A3"}, {"Skill A4"}, {"Skill A5"}, {"Skill A6"}}},
    {"Cyborg",    16 + 3*196, 510, 180, 80, {{"Skill C1"}, {"Skill C2"}, {"Skill C3"}, {"Skill C4"}, {"Skill C5"}, {"Skill C6"}}}
};

static int itemCount = 0; // Géré en interne ici pour alléger le menu

void InitGameData(void) {
    for (int i = 0; i < 4; i++) {
        characterButtons[i].logic.name = characterButtons[i].name;
        characterButtons[i].logic.maxHealth = 100;
        characterButtons[i].logic.stats[STAT_HP] = 100;
        characterButtons[i].logic.stats[STAT_ATK] = 15;
        characterButtons[i].logic.stats[STAT_DEF] = 7;
        characterButtons[i].logic.stats[STAT_SPD] = 10;
        characterButtons[i].logic.guarding = false;

        for (int j = 0; j < 6; j++) {
            characterButtons[i].skills[j].x = 250;
            characterButtons[i].skills[j].y = 100 + j * 60;
            characterButtons[i].skills[j].width = 300;
            characterButtons[i].skills[j].height = 45;
            
            characterButtons[i].skills[j].logic.name = characterButtons[i].skills[j].name;
            characterButtons[i].skills[j].logic.type = SKILL_NORMAL_DAMAGE;
            characterButtons[i].skills[j].logic.value = 10 + (j * 2);
            characterButtons[i].skills[j].logic.harm = true;
        }
    }
}

void UpdateMenuLogic(Vector2 mousePos, int* menuState, int* sceltaPersonaggio, Character* enemy, char* battleLog, int maxLogLen) {
    if (enemy->stats[STAT_HP] <= 0 && *menuState != 7) {
        *menuState = 7;
        return; // Interrompe la logica per questo frame ed evita di leggere i click del mouse
    }
    switch (*menuState) {
        case 0: { 
            if (CheckCollisionPointRec(mousePos, (Rectangle){ mainButtons[0].x, mainButtons[0].y, mainButtons[0].width, mainButtons[0].height }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = 1;
            if (CheckCollisionPointRec(mousePos, (Rectangle){ mainButtons[1].x, mainButtons[1].y, mainButtons[1].width, mainButtons[1].height }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = 3;
            if (CheckCollisionPointRec(mousePos, (Rectangle){ mainButtons[2].x, mainButtons[2].y, mainButtons[2].width, mainButtons[2].height }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = 4;
            if (CheckCollisionPointRec(mousePos, (Rectangle){ mainButtons[3].x, mainButtons[3].y, mainButtons[3].width, mainButtons[3].height }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = 6;
            break;
        }
        case 1: { 
            for (int i = 0; i < 4; i++) {
                if (CheckCollisionPointRec(mousePos, (Rectangle){ characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    *sceltaPersonaggio = i;
                    *menuState = 2;
                }   
            }
            break;
        }
        case 2: { 
            for (int j = 0; j < 6; j++) {
                Rectangle skillRect = { 
                    characterButtons[*sceltaPersonaggio].skills[j].x, 
                    characterButtons[*sceltaPersonaggio].skills[j].y, 
                    characterButtons[*sceltaPersonaggio].skills[j].width, 
                    characterButtons[*sceltaPersonaggio].skills[j].height 
                };
                if (CheckCollisionPointRec(mousePos, skillRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    ActionSkill(&characterButtons[*sceltaPersonaggio].logic, enemy, &characterButtons[*sceltaPersonaggio].skills[j].logic, battleLog, maxLogLen);
                    *menuState = 0;   
                }
            }
            break;
        }
        case 3: { 
            for (int i = 0; i < 4; i++) {
                if (CheckCollisionPointRec(mousePos, (Rectangle){ characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    ActionGuard(&characterButtons[i].logic, battleLog, maxLogLen);
                    *menuState = 0;   
                }
            }
            break;
        }
        case 4: { 
            for (int i = 0; i < 4; i++) {
                if (CheckCollisionPointRec(mousePos, (Rectangle){ characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = 5;   
            }
            break;
        }
        case 5: { 
            if (itemCount == 0) {
                if (CheckCollisionPointRec(mousePos, (Rectangle){ 350, 275, 100, 50 }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *menuState = 0;
            }
            break;
        }
        case 6: { 
            if (CheckCollisionPointRec(mousePos, (Rectangle){ 350, 240, 100, 50 }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                bool fugaRiuscita = ActionEscape(&characterButtons[0].logic, enemy, battleLog, maxLogLen);
                *menuState = fugaRiuscita ? 7 : 0; 
            }
            if (CheckCollisionPointRec(mousePos, (Rectangle){ 350, 310, 100, 50 }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                bool graziaRicevuta = ActionSpare(battleLog, maxLogLen);
                *menuState = graziaRicevuta ? 7 : 0;
            }
            break;
        }
        case 7: { 
            if (CheckCollisionPointRec(mousePos, (Rectangle){ 350, 275, 100, 50 }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                enemy->stats[STAT_HP] = enemy->maxHealth; 
                InitGameData();                          
                snprintf(battleLog, maxLogLen, "Scegli un'azione per iniziare."); 
                *menuState = 0;              
            }
            break;
        }
    }
}

void DrawMenuUI(int menuState, int sceltaPersonaggio, const Character* enemy, const char* battleLog) {
    DrawText(TextFormat("NEMICO: %s | HP: %d/%d", enemy->name, enemy->stats[STAT_HP], enemy->maxHealth), 30, 30, 22, RED);
    DrawText(battleLog, 30, 70, 18, WHITE);
    
    switch (menuState) {
        case 0:
            DrawRectangle(0, 500, 800, 400, GRAY);
            for (int i = 0; i < 4; i++) {
                DrawRectangle(mainButtons[i].x, mainButtons[i].y, mainButtons[i].width, mainButtons[i].height, LIGHTGRAY);
                DrawText(mainButtons[i].name, mainButtons[i].x + 20, mainButtons[i].y + 30, 20, BLACK);
            }
            break;
        case 1:
            DrawRectangle(0, 500, 800, 400, GRAY);
            for (int i = 0; i < 4; i++) {
                DrawRectangle(characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height, RED);
                DrawText(characterButtons[i].name, characterButtons[i].x + 20, characterButtons[i].y + 30, 20, BLACK);
            }
            break;
        case 2:
            DrawRectangle(230, 80, 340, 385, DARKGRAY);
            for (int j = 0; j < 6; j++) {
                struct SkillUI skillCorrente = characterButtons[sceltaPersonaggio].skills[j];
                DrawRectangle(skillCorrente.x, skillCorrente.y, skillCorrente.width, skillCorrente.height, RED);
                DrawText(skillCorrente.name, skillCorrente.x + 20, skillCorrente.y + 12, 20, BLACK);
            }
            break;
        case 3:
            DrawRectangle(0, 500, 800, 400, GRAY);
            for (int i = 0; i < 4; i++) {
                DrawRectangle(characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height, SKYBLUE);
                DrawText(characterButtons[i].name, characterButtons[i].x + 20, characterButtons[i].y + 30, 20, BLACK);
            }
            break;
        case 4:
            DrawRectangle(0, 500, 800, 400, GRAY);
            for (int i = 0; i < 4; i++) {
                DrawRectangle(characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height, GOLD);
                DrawText(characterButtons[i].name, characterButtons[i].x + 20, characterButtons[i].y + 30, 20, BLACK);
            }
            break;
        case 5:
            if (itemCount == 0) {
                DrawRectangle(230, 80, 340, 385, DARKGRAY);
                DrawText("There are no items left.", 260, 110, 20, WHITE);
                DrawRectangle(350, 275, 100, 50, GOLD);
                DrawText("OK", 385, 290, 20, BLACK);
            }
            break;
        case 6:
            DrawRectangle(300, 200, 200, 200, DARKGRAY);
            DrawRectangle(350, 240, 100, 50, WHITE);   DrawText("Fuggi", 375, 255, 20, BLACK);
            DrawRectangle(350, 310, 100, 50, WHITE);   DrawText("Grazia", 368, 325, 20, BLACK);
            break;
        case 7:
            DrawRectangle(230, 80, 340, 385, DARKGRAY);
            DrawText("You Won!", 260, 110, 20, WHITE);
            DrawRectangle(350, 275, 100, 50, GOLD);
            DrawText("OK", 385, 290, 20, BLACK);
            break;
    }
}