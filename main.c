#include "game_types.h" // this includes stdio and stdlib
#include "maps.h"
#include "render.h"

#define ENEMY_HEIGHT -5
#define HEART_HEIGHT -5

//#define DEBUG

#ifdef DEBUG
static const char db = TRUE;
#else
static const char db = FALSE;
#endif

typedef enum {
    TITLE_SCREEN = 0,
    PLAYING_GAME,
    END_SCREEN
} State; 
static State gamestate = TITLE_SCREEN;

static int dChange = -1;

static enum
{ // enum used in the handle keys function
    W = 87,
    A = 65,
    S = 83,
    D = 68,
    E = 69,
    COMMA = 44,
    PERIOD = 46,
    R = 82

} INPUT_KEYS;

static unsigned int buttonBuffer = 0x0;
// it doesn't matter if it's signed or not but I think it's funny how long the declaration is

static int flashTimer = 0; // this will be kept in main
static int bTravel; // set during the ray cast, vector for bullet travel
static char bulletTravel = FALSE; // this could be replaced with flashtimer

static int depth[120];

static char tutorial = TRUE;

void handleKeys(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            if (gamestate == PLAYING_GAME) gamestate = TITLE_SCREEN;
            else if (gamestate == TITLE_SCREEN) glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_ENTER:
            if (gamestate == TITLE_SCREEN) {
                gamestate = PLAYING_GAME;
                if (tutorial) {
                    printf("\n---Press 'W' to move forward and 'S' to move back---\n");
                    printf("---Press 'A' and 'D' to look left and right---\n");
                    tutorial = FALSE;
                }
            }
            break;
        case W:
            buttonBuffer ^= W_DOWN;
            break;
        case A:
            buttonBuffer ^= A_DOWN;
            break;
        case S:
            buttonBuffer ^= S_DOWN;
            break;
        case D:
            buttonBuffer ^= D_DOWN;
            break;
        case E:
            buttonBuffer ^= E_DOWN;
            break;
        case R:
            buttonBuffer ^= R_DOWN;
            break;
        case COMMA:
            if (gamestate == PLAYING_GAME) {
                buttonBuffer ^= COMMA_DOWN;
                buttonBuffer ^= GUN_FIRE;
            }
            break;
        case PERIOD:
            if (gamestate == PLAYING_GAME) { 
                buttonBuffer ^= PERIOD_DOWN; 
            }
            break;
        default:
            break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        switch (key)
        {
        case W:
            buttonBuffer ^= W_DOWN;
            break;
        case A:
            buttonBuffer ^= A_DOWN;
            break;
        case S:
            buttonBuffer ^= S_DOWN;
            break;
        case D:
            buttonBuffer ^= D_DOWN;
            break;
        case E:
            //buttonBuffer ^= E_DOWN;
            break;
        case COMMA:
            //buttonBuffer ^= COMMA_DOWN;
            break;
        case PERIOD:
            buttonBuffer ^= PERIOD_DOWN;
            break;
        default:
            break;
        }
    }
}

void levelInit(Sprite ** s) {
    
    Sprite* sp = *s; // create a copy of the head (basically just to avoid dereferencing every time)
    Sprite* spr; // sprite pointer for the loop
    // enemy height = -20

    // top left = 96, 96 -- add 64 per square


    sp = newSprite(COLLECTABLE, BOTTOM_LEFT, 1, COLLECTABLE_HEALTH, 96, 96, HEART_HEIGHT); // BL heart top left

    spr = spriteAdd(sp, COLLECTABLE, BOTTOM_LEFT, 1, COLLECTABLE_HEALTH, 355, 418, HEART_HEIGHT); // BL heart bottom right
    
    spr = spriteAdd(spr, COLLECTABLE, TOP_LEFT, 1, COLLECTABLE_HEALTH, 96, 160, HEART_HEIGHT); // TL heart top left

    spr = spriteAdd(spr, COLLECTABLE, TOP_LEFT, 1, COLLECTABLE_HEALTH, 96, 416, HEART_HEIGHT); // TL heart bottom left

    spr = spriteAdd(spr, ENEMY, TOP_LEFT, 13, ENEMY_HEALTH, 224, 288, ENEMY_HEIGHT); // TL enemy middle

    spr = spriteAdd(spr, ENEMY, TOP_RIGHT, 13, ENEMY_HEALTH, 96, 288, ENEMY_HEIGHT); // TR enemy left

    spr = spriteAdd(spr, COLLECTABLE, TOP_RIGHT, 1, COLLECTABLE_HEALTH, 96, 416, HEART_HEIGHT); // TR heart bottom left

    spr = spriteAdd(spr, COLLECTABLE, TOP_RIGHT, 1, COLLECTABLE_HEALTH, 352, 96, HEART_HEIGHT); // TR heart top tight

    spr = spriteAdd(spr, COLLECTABLE, BOTTOM_RIGHT, 1, COLLECTABLE_HEALTH, 224, 160, HEART_HEIGHT); // BR left heart

    spr = spriteAdd(spr, COLLECTABLE, BOTTOM_RIGHT, 1, COLLECTABLE_HEALTH, 224, 288, HEART_HEIGHT); // BR middle heart

    spr = spriteAdd(spr, COLLECTABLE, BOTTOM_RIGHT, 14, GUN_HEALTH, 96, 160, HEART_HEIGHT);

    // weapon location bottom right

    *s = sp;
}

void levelWipe() { return; } // will be called when changing maps
/*
This requires some refactoring, ideally a map could contain a list of the sprites that will exist per level
*/


int main()
{
    // START INITIALIZATION ------------------------------------

    // AUDIO INIT
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio error: %s\n", Mix_GetError());
        return 1;
    }

    Mix_Volume(-1, 24);

    // GRAPHICS INIT
    GLFWwindow *window;
    
    if (!glfwInit())
    {
        printf("OpenGL initialization failed\n");
        glfwTerminate(); // I don't know if this function can be called here, but I can always remove it
        return -1;
    }

    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // you can't resize the window
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Valentines Game", NULL, NULL); // name the window anything applicable 
    //960 512
    if (window == NULL)
    {
        printf("Could not create window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, handleKeys);
    

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {   
        printf("Couldn't load opengl\n");
        glfwTerminate();
        return -1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // default screen color
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // END INITIALIZATION ------------------------------------------------------

    Sprite * headSprite;
    levelInit(&headSprite); 
    // *s = newSprite(1,2,1,3,96,96,30);
    //headSprite->next = newSprite(1, 2, 1, 3, 380, 418, 20);

    Sprite* hS; // copies of the head for each list
    Map* hM;

    // While it's annoying to have such a long function header, this allows for dynamic creation of sprites
    Map * headMap = newMap(map_walls_small, map_walls_small_2, map_walls_small_3, map_walls_small_4);
    
    for (int i = 0; i < ROOM_NUM; i++) {
        headMap->c[i] = map_ceiling_small;
        headMap->f[i] = map_floor_small;
    }
    headMap->mapC = headMap->c[0];
    headMap->mapF = headMap->f[0];
    // this could be its own function
    
    Player player = {
        .plX = 100,
        .plY = 400,
        .pdX = (float)cos(0.0001) * VIEWING_ANGLE_CHANGE,
        .pdY = (float)sin(0.0001) * VIEWING_ANGLE_CHANGE,
        .hasGun = FALSE // will eventually start as false
    };

    double lastTime = glfwGetTime();
    double currentTime;
    double deltaTime;
   
    double animation = 0;
    int secondAnimation = 0;

    printf("Welcome to the game!!!\n\n");
    printf("---Press 'Enter' to start or press 'Esc' to pause the game---\n");
    printf("---(press 'Esc' once more in the pause screen to exit)---\n");
    
   
    // GAME LOOP

    while (!glfwWindowShouldClose(window))
    { // main loop
        currentTime = glfwGetTime();
        deltaTime = (float)(currentTime - lastTime);
        animation += deltaTime;
        if (animation >= 0.1) animation = 0;
        //printf("dt: %f\n", deltaTime);

        //printf("%lf\n", deltaTime);
        switch (gamestate) {

            case TITLE_SCREEN:
                glfwPollEvents();

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                drawSide(player);
                drawScreen(1);

                glfwSwapBuffers(window); // has to be last

                break;

            case PLAYING_GAME:
                
                hS = headSprite;
                hM = headMap;
                
                //taking input
                glfwPollEvents();
                
                int d = movePlayer(&player, deltaTime, headSprite, &buttonBuffer, hM, bTravel, &flashTimer);
                if (d != -1) {
                    dChange = d;
                    
                }
                
                if (buttonBuffer & DOOR_SLIDE) {
                    //printf("changed!, %d\n", d);
                    if (animation == 0) {
                        if (hM->map[dChange] == 4) {
                            hM->map[dChange] = 6;
                        }
                        else if (hM->map[dChange] == 11) {
                            hM->map[dChange] = 0;
                            buttonBuffer ^= DOOR_SLIDE;
                        }
                        else {
                            hM->map[dChange] = hM->map[dChange] + 1;
                        }
                    }
                }
                //printf("x: %f, y: %f\n", player.plX, player.plY);
                hS = headSprite;
                while (hS) {

                    moveSprite(hS, deltaTime, &player, *hM);
                        // it should be something like shootgun takes in the sprite and 
                        // runs a loop for each of the sprites, regardless of if they're NULL
                        // and bulletScan simply fires into the wall if so
                    
                    hS = hS->next;
                    
                }
                

                hS = headSprite;
                hM = headMap;
                bulletTravel = FALSE;
                
                //rendering 
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      
                
                drawRays3D(player, *hM, &bTravel, depth);
                drawSide(player);
                while (hS != NULL) {
                    if (animation == 0) {
                        if (hS->texture == 12) hS->texture = 1;
                        else hS->texture += 1;

                        if (hS->texture == 33) { 
                            hS->texture = 14; 
                            hS->z = HEART_HEIGHT;
                        }
                        else if (hS->texture >= 14){ 
                            hS->texture += 1; 
                            if (hS->texture % 10 < 5) {
                                hS->z -= 1;
                            }
                            if (hS->texture % 10 > 5) {
                                hS->z += 1;
                            }
                        }
                    }
                    drawSprite(hS, player, *hM, &flashTimer, &deltaTime, depth);   
                    hS = hS->next;
                }

                hS = headSprite;
                hM = headMap;

                if (db) {

                    drawMap2D(*hM); // gonna take in the current map
                
                    drawPlayer(player); 
                    drawBullet();

                    while (hS != NULL) {
                        drawSpriteMap(*hS);
                        hS = hS->next;
                    }

                    hS = headSprite;
                    hM = headMap;
                    
                }
                //else {
                    // if we are not in debug mode, clear the memory
                /*
                    hS = headSprite;

                    while (hS) { // free all sprites in state 0
                        Sprite* nextSprite = hS->next; // Store next before removal
                        //printf("headsprite x: %f, headsprite y: %f\n", hS->x, hS->y);
                        if (hS->state == 0) {
                            //if (hS == headSprite) {
                              //  headSprite = headSprite->next;
                            //}
                            
                            spriteRemove(hS, NULL, &headSprite);
                            hS = NULL;
                            
                        }

                        hS = nextSprite; // Move to next sprite
                    }

                */   
                //}
                
                drawGun(player, buttonBuffer);
                if (animation == 0 && buttonBuffer & GUN_FIRE) {
                    secondAnimation += 1;
                    if (secondAnimation == 2) {
                        buttonBuffer ^= GUN_FIRE;
                        secondAnimation = 0;
                    }
                    
                }
                glfwSwapBuffers(window); 

                break;

            case END_SCREEN:

                break;
        }

        lastTime = currentTime;
        
    }

    Mix_CloseAudio();
    SDL_Quit();
    glfwTerminate(); // get rid of the window at the end

    return 0;
}
