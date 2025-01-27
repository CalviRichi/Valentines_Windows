#include "game_types.h" // this includes stdio and stdlib
#include "maps.h"
#include "render.h"

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
static State gamestate = PLAYING_GAME;

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

static unsigned char buttonBuffer = 0x0;
// it doesn't matter if it's signed or not but I think it's funny how long the declaration is

static int flashTimer = 0; // this will be kept in main
static int bTravel; // set during the ray cast, vector for bullet travel
static char bulletTravel = FALSE; // this could be replaced with flashtimer

static int depth[120];

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
            buttonBuffer ^= COMMA_DOWN;
            break;
        case PERIOD:
            buttonBuffer ^= PERIOD_DOWN;
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
            buttonBuffer ^= E_DOWN;
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
    Sprite* sp = *s;
    sp = newSprite(1, 2, 1, 3, 96, 96, -5);
    sp->next = newSprite(1, 2, 1, 3, 380, 418, -5);
    sp->next->previous = sp;
    sp->next->next = newSprite(1, 2, 1, 3, 160, 120, -5);
    sp->next->next->previous = sp->next;
    *s = sp;
}
/*
This requires some refactoring, ideally a map could contain a list of the sprites that will exist per level

ADD SPRITES THAT EXIST ON OTHER MAPS
*/

int main()
{
    // START INITIALIZATION ------------------------------------

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
        .hasGun = TRUE // will eventually start as false
    };

    double lastTime = glfwGetTime();
    double currentTime;
    double deltaTime;

    

    /*
    the main test:
    create a 
    */

    double animation = 0;

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

                drawScreen(1);

                glfwSwapBuffers(window); // has to be last

                break;

            case PLAYING_GAME:
                
                hS = headSprite;
                hM = headMap;
                
                //taking input
                glfwPollEvents();
                
                movePlayer(&player, deltaTime, headSprite, &buttonBuffer, hM, bTravel, &flashTimer);
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
                        if (hS->map == 12) hS->map = 1;
                        else hS->map += 1;
                    }
                    drawSprite(hS, player, *hM, &flashTimer, depth);   
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

                drawGun(player);
                glfwSwapBuffers(window); 

                break;

            case END_SCREEN:

                break;
        }

        lastTime = currentTime;
        
    }

    glfwTerminate(); // get rid of the window at the end

    return 0;
}
