#ifndef GAME_DRAW_H
#define GAME_DRAW_H

#include "game_types.h"

#include "dependencies/include/glad/glad.h"
#include "dependencies/include/glfw3.h"


/*
All of these will need to take in parameters, mostly the map and player coordinates. 
This is just an idea for creating ease of readablility within these files.

this should be an exercise in creating readable headers 
*/




/*
Used for debugging, draws a 2D representation of the map on the left side of the screen



/*
Draws a texture (yet to be implemented) onto a portion of the screen
@param lineO 
 -- the beginning of the wall portion of the screen
@param lineH
 -- when added to lineO, denotes the end of the wall portion
@param shade
 -- used to add shading to the colors when drawing textures
*/
void drawMap3D(float lineH, float lineO, float shade);

/*
Used in tandem with the drawMap2D function, adds player to the 2D map

@param p
 -- Player struct for the player's x, y coordinates
*/
void drawPlayer(Player * p);


#endif // GAME_DRAW_H