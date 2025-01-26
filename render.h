#ifndef RENDER_H
#define RENDER_H

#include "game_types.h"

void drawSide();

void drawRays3D(Player p, Map map, int * bTravel, int depth[120]);

void drawGun();

void drawSprite(Sprite* sp, Player p, Map m, int* flashTimer, int depth[120]);

void screen(int v);
#endif