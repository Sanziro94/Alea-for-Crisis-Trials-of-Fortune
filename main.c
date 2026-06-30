#include "raylib.h"
#include "gameplay.h"
#include "ui.h"
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL)); 
    InitWindow(800, 600, "Alea for Crisis : Trial of Fortune");
    SetTargetFPS(60);
    
    int menuState = 0;
    int selectedCharacter = 0;
    char battleLog[512] = "Choose an action to begin.";

    InitGameData();

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        
        UpdateMenuLogic(mousePos, &menuState, &selectedCharacter, &enemy, battleLog, sizeof(battleLog));
        
        BeginDrawing();
            ClearBackground(DARKGREEN);
            DrawMenuUI(menuState, selectedCharacter, &enemy, battleLog);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
