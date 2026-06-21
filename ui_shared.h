#ifndef UI_SHARED_H
#define UI_SHARED_H

#include "raylib.h"
#include "gameplay.h"

struct Buttons {
    char name[20];
    int x, y, width, height;
};

struct SkillUI {
    char name[20];
    int x, y, width, height;
    Skill logic; 
};

struct CharacterUI {
    char name[20];
    int x, y, width, height;
    struct SkillUI skills[6];
    Character logic; 
};

struct Item {
    char name[20];
    int x, y, width, height;
};

struct Inventario {
    struct Item items[10];
    int itemCount;
};

extern struct Buttons mainButtons[4];
extern struct CharacterUI characterButtons[4];

// Fonctions globales pour simplifier le menu principal
void InitGameData(void);
void UpdateMenuLogic(Vector2 mousePos, int* menuState, int* sceltaPersonaggio, Character* enemy, char* battleLog, int maxLogLen);
void DrawMenuUI(int menuState, int sceltaPersonaggio, const Character* enemy, const char* battleLog);

#endif