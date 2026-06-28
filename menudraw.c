#include "menudraw.h"
#include "raylib.h"
#include <stdio.h>

void DrawMenuButtons(void){
    DrawRectangle(0, 500, 800, 100, DARKGRAY);
    for (int i = 0; i < 4; i++) {
        DrawRectangleRec(mainButtons[i].rect, LIGHTGRAY);
        DrawRectangleLinesEx(mainButtons[i].rect, 1, BLACK);
        DrawText(mainButtons[i].name, mainButtons[i].rect.x + 40, mainButtons[i].rect.y + 30, 20, BLACK);
    }
}
void DrawCharacterButtons(Color baseColor, Color textColor) {
    DrawRectangle(0, 500, 800, 100, DARKGRAY);
    for (int i = 0; i < 4; i++) {
        DrawRectangleRec(characterButtons[i].rect, baseColor);
        DrawRectangleLinesEx(characterButtons[i].rect, 1, textColor);
        DrawText(characterButtons[i].name, characterButtons[i].rect.x + 20, characterButtons[i].rect.y + 30, 20, textColor);
    }
}
void DrawSkill(int selectedCharacter) {
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
}
void DrawItems(void) {
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
}
void DrawEscape(void) {
    DrawRectangle(250, 200, 300, 220, DARKGRAY);
    DrawRectangleLines(250, 200, 300, 220, WHITE);
            
    DrawRectangle(350, 240, 100, 40, LIGHTGRAY);
    DrawText("Escape", 360, 250, 18, BLACK);

    DrawRectangle(350, 310, 100, 40, LIGHTGRAY);
    DrawText("Spare", 365, 320, 18, BLACK);
}
void DrawClash(int selectedCharacter){
    DrawRectangle(100, 380, 600, 200, BLACK);
    DrawRectangleLines(100, 380, 600, 200, RED);
    DrawText("!! ATTACK CLASH !!", 295, 400, 24, ORANGE);
            
    for (int i = 0; i < SEQUENCE_LENGTH; i++) {
        Color stepColor = WHITE;
        if (i < currentStep) stepColor = GREEN;      
            else if (i == currentStep) stepColor = YELLOW;        
        if (sequenceNames[i] != NULL) DrawText(sequenceNames[i], 160 + (i * 100), 450, 22, stepColor);
    }        
    float maxClashTime = 2.5f + (characterButtons[selectedCharacter].logic.stats[STAT_SPD] * 0.1f);
    DrawRectangle(200, 520, (int)(400 * (timeRemaining / maxClashTime)), 15, RED);
    DrawRectangleLines(200, 520, 400, 15, WHITE);
}
void DrawMenuMain(int selectedCharacter) {
    DrawMenuButtons();
}
void DrawMenuSelectAttack(int selectedCharacter) {
    DrawCharacterButtons(RED, WHITE);
}
void DrawMenuSelectSkill(int selectedCharacter) {
    DrawSkill(selectedCharacter);
}
void DrawMenuSelectGuard(int selectedCharacter) {
    DrawCharacterButtons(SKYBLUE, BLACK);
}
void DrawMenuSelectBag(int selectedCharacter) {
    DrawCharacterButtons(GOLD, BLACK); 
}
void DrawMenuBagItems(int selectedCharacter) {
    DrawItems();
}
void DrawMenuEscapeSpare(int selectedCharacter) {
    DrawEscape();
}
void DrawMenuClash(int selectedCharacter) {
    DrawClash(selectedCharacter);
}
void (*MenuDrawTable[])(int) = {
    DrawMenuMain,       
    DrawMenuSelectAttack,  
    DrawMenuSelectSkill,  
    DrawMenuSelectGuard,  
    DrawMenuSelectBag,    
    DrawMenuBagItems,
    DrawMenuEscapeSpare,  
    DrawMenuClash 
};

