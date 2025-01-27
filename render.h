#ifndef RENDER_H
#define RENDER_H

#include "game_types.h"

void drawSide(Player p);

void drawRays3D(Player p, Map map, int * bTravel, int depth[120]);

void drawGun(Player p);

void drawSprite(Sprite* sp, Player p, Map m, int* flashTimer, int depth[120]);

void drawScreen(int v); // draws a full screen of a given image
#endif