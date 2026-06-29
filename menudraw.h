#ifndef MENUDRAW_H
#define MENUDRAW_H

#include "raylib.h"
#include "ui.h"
#include "gameplay.h"

void DrawCharacterButtons(int selectedCharacter);
void DrawActionButtons(int selectedCharacter);
void DrawSkill(int selectedCharacter);
void DrawItems(int selectedCharacter);
void DrawEscape(int selectedCharacter);
void DrawClash(int selectedCharacter);
void DrawSafety(int selectedCharcater);

extern void (*MenuDrawTable[])(int);

#endif