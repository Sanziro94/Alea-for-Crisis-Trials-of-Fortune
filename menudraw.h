#ifndef MENUDRAW_H
#define MENUDRAW_H

#include "raylib.h"
#include "ui.h"
#include "gameplay.h"

void DrawMenuMain(int selectedCharacter);
void DrawMenuSelectAttack(int selectedCharacter);
void DrawMenuSelectSkill(int selectedCharacter);
void DrawMenuSelectGuard(int selectedCharacter);
void DrawMenuSelectBag(int selectedCharacter);
void DrawMenuBagItems(int selectedCharacter);
void DrawMenuEscapeSpare(int selectedCharacter);
void DrawMenuEmpty(int selectedCharacter);
void DrawMenuClash(int selectedCharacter);
void DrawSafety(int selectedCharcater);

extern void (*MenuDrawTable[])(int);

#endif