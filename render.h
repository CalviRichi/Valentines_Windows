#ifndef RENDER_H
#define RENDER_H

#include "game_types.h"

// ------------- RENDER FUNCTIONS -- DEFINED WITHIN RENDER.C

void drawSide(Player p);

void drawRays3D(Player p, Map map, int * bTravel, int depth[120]);

void drawGun(Player p, static unsigned int bb);

void drawHeart(Heart h);

void drawSprite(Sprite* sp, Player p, Map m, int* flashTimer, int dt, int depth[120]);

void drawScreen(int v); // draws a full screen of a given image

#endif