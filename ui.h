#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "gameplay.h"

typedef enum { PLAYER_TURN_STATE, ENEMY_TURN_STATE, ROUND_END_STATE} States;
typedef enum {
    MENU_MAIN = 0,
    MENU_SELECT_CHAR_ATTACK,
    MENU_SELECT_SKILL,
    MENU_SELECT_CHAR_GUARD,
    MENU_SELECT_CHAR_BAG,
    MENU_BAG_ITEMS,
    MENU_ESCAPE_SPARE,
    MENU_ESCAPED,
    MENU_CLASH,
    MENU_MERCY,
    MENU_VICTORY,
    MENU_DEFEAT
} MenuState;

struct Buttons {
    char name[20];
    Rectangle rect;
};

struct SkillUI {
    char name[20];
    Rectangle rect;
    Skill logic; 
};

struct CharacterUI {
    char name[20];
    Rectangle rect;
    struct SkillUI skills[6];
    Character logic; 
};

struct ObjectUI {
    char name[20];
    Rectangle rect;
    int itemId;
    int quantity;
};

extern struct Buttons mainButtons[4];
extern struct CharacterUI characterButtons[4];

// UI and menu management functions
void InitGameData(void);
void UpdateMenuLogic(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen);
void DrawMenuUI(int menuState, int selectedCharacter, const Character* enemy, const char* battleLog);

#endif
