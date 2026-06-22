#include "ui_shared.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

struct ObjectUI borsa[5];
int tipiOggettiInBorsa = 5;


void InitGameData(void) {
    // Mercenary
    characterButtons[0].logic = (Character){"Mercenary", {100, 12, 8, 5}, 100, false};
    // Elf
    characterButtons[1].logic = (Character){"Elf", {80, 10, 5, 12}, 80, false};
    // Assassin
    characterButtons[2].logic = (Character){"Assassin", {75, 15, 4, 15}, 75, false};
    // Cyborg
    characterButtons[3].logic = (Character){"Cyborg", {120, 14, 10, 4}, 120, false};

    // Assegnazione coordinate spaziali dei rettangoli per le Skill
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
            characterButtons[i].skills[j].x = 40 + (j % 3) * 240;
            characterButtons[i].skills[j].y = 420 + (j / 3) * 80;
            characterButtons[i].skills[j].width = 220;
            characterButtons[i].skills[j].height = 60;
            characterButtons[i].skills[j].logic = (Skill){characterButtons[i].skills[j].name, SKILL_NORMAL_DAMAGE, 5 + j, true, STAT_HP};
        }
    }
    // Assegnazione coordinate spaziali dei rettangoli per gli Oggetti
    for (int i = 0; i < 5; i++) {
        borsa[i].x = 40 + (i % 3) * 240; 
        borsa[i].y = 420 + (i / 3) * 80; 
        borsa[i].width = 220;
        borsa[i].height = 60;
        borsa[i].idOggetto = i; 
        snprintf(borsa[i].name, sizeof(borsa[i].name), "%s", objectTable[i].name);
        if (i == 0) {
            borsa[i].quantita = 2; 
        } else {
            borsa[i].quantita = 1;
        }
    }
}

void UpdateMenuLogic(Vector2 mousePos, int* menuState, int* sceltaPersonaggio, Character* enemy, char* battleLog, int maxLogLen) {
    if (enemy->stats[STAT_HP] <= 0 && *menuState != 7) {
        *menuState = 7;
        return;
    }
    bool tuttiMorti = true;
    for (int i = 0; i < 4; i++) {
        if (characterButtons[i].logic.stats[STAT_HP] > 0 ) {
            tuttiMorti = false;
            break;
        }   
    }
    if (tuttiMorti && *menuState != 7)
    {
        *menuState = 7;
        return;
    }
    // Gestione dello stato di Clash isolato (Stato 8)
    if (*menuState == 8) {
        bool clashFinito = ActionClash(&characterButtons[*sceltaPersonaggio].logic, enemy, battleLog, maxLogLen);
        if (clashFinito) {
            *menuState = 0; 
        }
        return;
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
                if (characterButtons[i].logic.stats[STAT_HP] > 0) {
                    if (CheckCollisionPointRec(mousePos, (Rectangle){ characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        *sceltaPersonaggio = i;
                        *menuState = 2;
                    }
                }
            }
            break;
        }
        case 2: { 
            for (int j = 0; j < 6; j++) {
                Rectangle r = { 
                    characterButtons[*sceltaPersonaggio].skills[j].x, 
                    characterButtons[*sceltaPersonaggio].skills[j].y, 
                    characterButtons[*sceltaPersonaggio].skills[j].width, 
                    characterButtons[*sceltaPersonaggio].skills[j].height 
                };
                
                if (CheckCollisionPointRec(mousePos, r) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    // Innesco casuale del Clash (15% di probabilità)
                    if (rand() % 100 < 15) {
                        *menuState = 8;
                        return;
                    }

                    // Turno Dinamico Reattivo (Iniziativa)
                    int priority = CheckPriority(&characterButtons[*sceltaPersonaggio].logic, enemy);
                    if (priority == 0 || (priority == 2 && rand() % 2 == 0)) {
                        ActionSkill(&characterButtons[*sceltaPersonaggio].logic, enemy, &characterButtons[*sceltaPersonaggio].skills[j].logic, battleLog, maxLogLen);
                        EnemyTurn(enemy, &characterButtons[*sceltaPersonaggio].logic, battleLog, maxLogLen);
                    } else {
                        snprintf(battleLog, maxLogLen, "%s è più rapido e intercetta l'azione!", enemy->name);
                        EnemyTurn(enemy, &characterButtons[*sceltaPersonaggio].logic, battleLog, maxLogLen);
                        
                        char playerLog[128];
                        ActionSkill(&characterButtons[*sceltaPersonaggio].logic, enemy, &characterButtons[*sceltaPersonaggio].skills[j].logic, playerLog, sizeof(playerLog));
                        strncat(battleLog, "\n", maxLogLen - strlen(battleLog) - 1);
                        strncat(battleLog, playerLog, maxLogLen - strlen(battleLog) - 1);
                    }
                    *menuState = 0;   
                }
            }
            break;
        }
        case 3: {
            for (int i = 0; i < 4; i++) {
                if (characterButtons[i].logic.stats[STAT_HP] > 0) {
                    if (CheckCollisionPointRec(mousePos, (Rectangle){ characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    ActionGuard(&characterButtons[i].logic, battleLog, maxLogLen);
                    EnemyTurn(enemy, &characterButtons[i].logic, battleLog, maxLogLen);
                    *menuState = 0;
                    }
                }
            }
            break;
        }
        case 4: {
            for (int i = 0; i < 4; i++) {
                if (characterButtons[i].logic.stats[STAT_HP] > 0) {
                    if (CheckCollisionPointRec(mousePos, (Rectangle){ characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    *sceltaPersonaggio = i;
                    *menuState = 5;
                    }
                }
            }
            break;
        }
        case 5: {
            for (int i = 0; i < 5; i++) {
                if (borsa[i].quantita > 0) {
                    Rectangle itemRect = { borsa[i].x, borsa[i].y, borsa[i].width, borsa[i].height };
                    if (CheckCollisionPointRec(mousePos, itemRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        ActionUseObject(borsa[i].idOggetto, &characterButtons[*sceltaPersonaggio].logic, enemy, battleLog, maxLogLen);
                        borsa[i].quantita--;
                        EnemyTurn(enemy, &characterButtons[*sceltaPersonaggio].logic, battleLog, maxLogLen);
                        *menuState = 0;
                    }
                }
            }
            Rectangle backRect = { 40 + (5 % 3) * 240, 420 + (5 / 3) * 80, 220, 60 };
            if (CheckCollisionPointRec(mousePos, backRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
                *menuState = 0;
            break;
         }
        case 6: {
            if (CheckCollisionPointRec(mousePos, (Rectangle){350, 250, 100, 40}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (ActionEscape(&characterButtons[0].logic, enemy, battleLog, maxLogLen)) {
                    *menuState = 7;
                } else {
                    *menuState = 0;
                }
            }
            if (CheckCollisionPointRec(mousePos, (Rectangle){350, 310, 100, 40}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (ActionSpare(battleLog, maxLogLen)) {
                    *menuState = 7;
                } else {
                    *menuState = 0;
                }
            }
            break;
        }
    }
}

void DrawMenuUI(int menuState, int sceltaPersonaggio, const Character* enemy, const char* battleLog) {
    // Info del nemico costanti sullo schermo
    DrawText(enemy->name, 50, 50, 22, RED);
    DrawText(TextFormat("HP: %d/%d", enemy->stats[STAT_HP], enemy->maxHealth), 50, 80, 20, LIGHTGRAY);

    // Box del log di battaglia
    DrawRectangle(50, 150, 700, 100, DARKGRAY);
    DrawRectangleLines(50, 150, 700, 100, WHITE);
    DrawText(battleLog, 65, 165, 16, WHITE);

    // Box degli Eroi
    for(int i = 0; i < 4; i++) {
        DrawText(TextFormat("%s HP: %d", characterButtons[i].name, characterButtons[i].logic.stats[STAT_HP]), 50 + i*180, 300, 18, GREEN);
    }

    switch (menuState) {
        case 0:
            for (int i = 0; i < 4; i++) {
                DrawRectangle(mainButtons[i].x, mainButtons[i].y, mainButtons[i].width, mainButtons[i].height, LIGHTGRAY);
                DrawRectangleLines(mainButtons[i].x, mainButtons[i].y, mainButtons[i].width, mainButtons[i].height, BLACK);
                DrawText(mainButtons[i].name, mainButtons[i].x + 40, mainButtons[i].y + 30, 20, BLACK);
            }
            break;
        case 1:
            for (int i = 0; i < 4; i++) {
                DrawRectangle(characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height, RED);
                DrawText(characterButtons[i].name, characterButtons[i].x + 20, characterButtons[i].y + 30, 20, WHITE);
            }
            break;
        case 2:
            DrawRectangle(0, 400, 800, 200, DARKGRAY);
            for (int j = 0; j < 6; j++) {
                Rectangle r = { characterButtons[sceltaPersonaggio].skills[j].x, characterButtons[sceltaPersonaggio].skills[j].y, characterButtons[sceltaPersonaggio].skills[j].width, characterButtons[sceltaPersonaggio].skills[j].height };
                DrawRectangleRec(r, MAROON);
                DrawRectangleLines(r.x, r.y, r.width, r.height, WHITE);
                DrawText(characterButtons[sceltaPersonaggio].skills[j].name, r.x + 15, r.y + 20, 18, WHITE);
            }
            break;
        case 3:
            for (int i = 0; i < 4; i++) {
                DrawRectangle(characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height, SKYBLUE);
                DrawText(characterButtons[i].name, characterButtons[i].x + 20, characterButtons[i].y + 30, 20, BLACK);
            }
            break;
        case 4:
            for (int i = 0; i < 4; i++) {
                DrawRectangle(characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height, GOLD);
                DrawText(characterButtons[i].name, characterButtons[i].x + 20, characterButtons[i].y + 30, 20, BLACK);
            }
            break;
        case 5:
            for (int i = 0; i < 5; i++) {
                if (borsa[i].quantita > 0) {
                    DrawRectangle(borsa[i].x, borsa[i].y, borsa[i].width, borsa[i].height, LIGHTGRAY);
                    DrawRectangleLines(borsa[i].x, borsa[i].y, borsa[i].width, borsa[i].height, BLACK);
                    DrawText(TextFormat("%s (x%d)", borsa[i].name, borsa[i].quantita), borsa[i].x + 15, borsa[i].y + 20, 18, BLACK);
                } else {
                    DrawRectangle(borsa[i].x, borsa[i].y, borsa[i].width, borsa[i].height, GRAY);
                    DrawRectangleLines(borsa[i].x, borsa[i].y, borsa[i].width, borsa[i].height, DARKGRAY);
                    DrawText("Esaurito", borsa[i].x + 15, borsa[i].y + 20, 18, DARKGRAY);
                }
            }
            int backX = 40 + (5 % 3) * 240;
            int backY = 420 + (5 / 3) * 80;
            DrawRectangle(backX, backY, 220, 60, RED);
            DrawRectangleLines(backX, backY, 220, 60, WHITE);
            DrawText("INDIETRO", backX + 55, backY + 20, 18, WHITE);  
            break;
        case 6:
            DrawRectangle(250, 200, 300, 220, DARKGRAY);
            DrawRectangleLines(250, 200, 300, 220, WHITE);
            
            DrawRectangle(350, 240, 100, 40, LIGHTGRAY);
            DrawText("Scappa", 365, 250, 18, BLACK);

            DrawRectangle(350, 310, 100, 40, LIGHTGRAY);
            DrawText("Risparmia", 355, 320, 18, BLACK);
            break;
        case 7:
            DrawRectangle(0, 0, 800, 600, BLACK);
            DrawText("BATTAGLIA CONCLUSA", 240, 250, 32, GOLD);
            break;
        case 8: // Finestra Grafica del QTE Clash
            DrawRectangle(100, 380, 600, 200, BLACK);
            DrawRectangleLines(100, 380, 600, 200, RED);
            DrawText("!! IMPATTO DI ATTACCHI !!", 260, 400, 24, ORANGE);
            
            for (int i = 0; i < SEQUENCE_LENGTH; i++) {
                Color stepColor = WHITE;
                if (i < currentStep) stepColor = GREEN;      
                else if (i == currentStep) stepColor = YELLOW; 
                
                if (sequenceNames[i] != NULL) {
                    DrawText(sequenceNames[i], 160 + (i * 100), 450, 22, stepColor);
                }
            }
            
            float maxClashTime = 2.5f + (characterButtons[sceltaPersonaggio].logic.stats[STAT_SPD] * 0.1f);
            DrawRectangle(200, 520, (int)(400 * (timeRemaining / maxClashTime)), 15, RED);
            DrawRectangleLines(200, 520, 400, 15, WHITE);
            break;
    }
}