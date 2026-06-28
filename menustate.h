#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "ui.h"

void MenuMain(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen);
void MenuFight(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen);
void MenuSkills(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen);
void MenuGuard(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen);
void MenuBag(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen);
void MenuItems(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen);
void MenuEscape(Vector2 mousePos, int* menuState, int* selectedCharacter, Character* enemy, char* battleLog, int maxLogLen);

extern void (*MenuStateTable[])(Vector2, int* , int* , Character* , char* , int);
#endif