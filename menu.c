#include "raylib.h"
#include "gameplay.h"
#include "ui_shared.h"
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL)); 
    InitWindow(800, 600, "Alea for Crisis : Trial of Fortune");
    SetTargetFPS(60);
    
    int menuState = 0;
    int sceltaPersonaggio = 0;
    char battleLog[512] = "Scegli un'azione per iniziare.";

    Character enemy = {
        .name = "Boss Goblin",
        .maxHealth = 300,
        .stats = {300, 24, 5, 8}, // HP, ATK, DEF, SPD
        .guarding = false
    };


    InitGameData();

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        
        
        UpdateMenuLogic(mousePos, &menuState, &sceltaPersonaggio, &enemy, battleLog, sizeof(battleLog));
        
        BeginDrawing();
            ClearBackground(DARKGREEN);
            DrawMenuUI(menuState, sceltaPersonaggio, &enemy, battleLog);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}