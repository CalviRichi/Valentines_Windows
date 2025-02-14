#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
//#include "dependencies/assets/textures.h"
#include "dependencies/include/glad/glad.h"
#include "dependencies/include/glfw3.h"
#include "dependencies/include/SDL2/include/SDL.h"
#include "dependencies/include/SDL2_mixer/include/SDL_mixer.h"


#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#define SCALE_X 120
#define SCALE_Y 80

#define TRUE 1 // boolean value true
#define FALSE 0 // boolean value false

#define SHIFT 4
#define SIDE_ADD 100 // 100
#define WINDOW_OFFSET 100 // 100 offset for drawing the screen
#define SCREEN_WIDTH (WINDOW_OFFSET + 960 + SIDE_ADD)//960 -- 
#define SCREEN_HEIGHT 640//640 or for a more "traditional" ratio, 512
#define STRETCH 8 // the width of each pixel

#define RAY_NUM 120

#define ROOM_NUM 4 // a definition for the number of rooms per map
#define SMALL_MAP // definition for size of map

#ifdef SMALL_MAP
    #define MAP_X 8 // number of tiles in the x direction
    #define MAP_Y 8 // number of tiles in the y direction
#else
    #define MAP_X 16
    #define MAP_Y 16
#endif

#define DOF MAP_X
#define MAP_S (MAP_X * MAP_Y) // area of a given room

#define PLAYER_SPEED 35 // the constant in the player move speed
#define SPRITE_SPEED 30 // speed of a sprites
#define VIEWING_ANGLE_CHANGE 5 // just don't touch this
#define PI 3.14159265335
#define P2 PI/2 // pi over 2 
#define P3 3*PI/2 // 3 pi over 2
#define DR 0.0174533 // one degree in radians 
#define MIN_DISTANCE 30 // the minimum distance to a wall

typedef enum
{ // enum used to set button buffer
    W_DOWN = 0x1,
    A_DOWN = 0x2,
    S_DOWN = 0x4,
    D_DOWN = 0x8,
    E_DOWN = 0x10,
    COMMA_DOWN = 0x20,
    PERIOD_DOWN = 0x40,
    R_DOWN = 0x80,
    DOOR_SLIDE = 0x100,
    GUN_FIRE = 0x200,
    ADD_HEART = 0x400,
    REMOVE_HEART = 0x800

} ACTION_KEYS;

// ----------- MAPS -------------

typedef struct Map{
    // a map is a doubly linked list that contains an array of level segments,
    // which are 8 x 8 sectors connected by walls of type 5
    struct Map * previous;
    int * m[ROOM_NUM]; // the map of all the walls
    int * map; // pointer to the current walls
    int * c[ROOM_NUM]; // the map of all the ceilings
    int * mapC; // the map of the current ceiling
    int * f[ROOM_NUM]; // the map of all the floors
    int * mapF; // the map of the current floor
    struct Map * next;
} Map;

// --------- PLAYER ------------

typedef struct {
    float plX, plY; // x and y coordinates
    float pdX, pdY, pa; // directions in each axis and an angle for the raycaster
    char hasGun; // whether or not the player has a gun
    float moveCounter;
    int heartCounter;
    /*
    Inventory
    Has gun?

    */
} Player;

// ---------- SPRITE ---------------

typedef enum {
    OFF = 0,
    ENEMY,
    COLLECTABLE
} SPRITE_TYPE;

enum HEALTH_INFO {
    NO_HEALTH = 0,
    ENEMY_HEALTH = 3,
    COLLECTABLE_HEALTH = -1,
    GUN_HEALTH = -2
};

typedef enum {
    BOTTOM_LEFT = 0,
    BOTTOM_RIGHT,
    TOP_RIGHT,
    TOP_LEFT
} REGION;

enum AUDIO_CHANNELS {
    GUNSHOT = 1,
    ITEM,
    STEP
};

typedef struct Sprite {
    // another doubly linked list
    struct Sprite * previous; 
    REGION region; // Where on the map the sprite is
    SPRITE_TYPE state; // describes whether sprite is "active" -> state > 0
                       // as well as the type of sprite -> enemy, collectable, etc...
    int texture; // What texture is displayed
    float x, y, z; // position for rendering
    int health; // self explanatory 
    struct Sprite * next;

} Sprite;

// ------------- HEART OBJECTS ---------------

enum COLORS {
    PURPLE,
    PINK,
    RED
};

typedef struct Heart {
    struct Heart* previous;
    float x_pos;
    float y_pos;
    char x_direction;
    char y_direction;
    int color;
    float speed;
    struct Heart* next;
} Heart;

#define HEART_X_MAX 183
#define HEART_X_MIN 18

#define HEART_Y_MAX 99
#define HEART_Y_MIN -6

#define HEART_SPEED 1
#define HEART_SIZE 5

// ------------- UTILITY FUNCTIONS -- TO BE USED IN VARIOUS FILES
static float dist(float ax, float ay, float bx, float by, float ang) {
    return (sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay)));
}

static int playSoundEffect(char* s, int channel) {

    Mix_Chunk* sound = Mix_LoadWAV(s);
    if (!sound) {
        printf("Failed to load sound: %s\n", Mix_GetError());
        return 1;
    }

    // Play the sound on the first available channel (-1)

    Mix_PlayChannel(channel, sound, 0);
    return 0;
}

// ------------- MAP FUNCTIONS -- DEFINED WITHIN MAP.C

void drawMap2D(Map m);
Map *newMap(int bottomleft[MAP_S], int bottomright[MAP_S], int topright[MAP_S], int topleft[MAP_S]);
Map *mapAdd(Map *tail, int bottomleft[MAP_S], int bottomright[MAP_S], int topright[MAP_S], int topleft[MAP_S]);
void mapRemove(Map *item, int * dataCopy[ROOM_NUM]);

// ------------- PLAYER FUNCTIONS -- DEFINED WITHIN PLAYER.C

void shootGun(Sprite * s, Player * p, int bTravel, int * flashTimer, Map * m);
void drawBullet();
void bulletScan(Sprite * s, Player * p, int bTravel, int * flashTimer, Map * m);
int movePlayer(Player* p, float deltaTime, Sprite* s, unsigned int* buttonBuffer, Map* m, int bTravel, int* flashTimer);
void drawPlayer(Player p);

// ------------- SPRITE FUNCTIONS -- DEFINED WITHIN SPRITE.C

Sprite *newSprite(SPRITE_TYPE state, int region, int texture, int health, float x, float y, float z);
Sprite *spriteAdd(Sprite * tail, SPRITE_TYPE state, int region, int texture, int health, float x, float y, float z);
void spriteRemove(Sprite * item, Sprite * sCopy, Sprite ** headSprite);
void drawSpriteMap(Sprite s);
int moveSprite(Sprite * s, float dt, Player * p, Map m);

// ------------- HEART OBJECT FUNCTIONS -- DEFINED WITHIN HEART_OBJECT.C

Heart* newHeart(float x_pos, float y_pos);
Heart* HeartAdd(Heart* tail);
void heartMove(Heart* h, char heartMoving, char heartFlip);
void addAHeart(Heart* h, static unsigned int * buttonBuffer);
void HeartRemove(Heart* sCopy, Heart** headHeart);

#endif