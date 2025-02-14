#include "game_types.h" // this includes stdio and stdlib
#include "maps.h"
#include "render.h"

#define ENEMY_HEIGHT -5
#define HEART_HEIGHT -5

//#define DEBUG

#ifdef DEBUG
static char db = TRUE;
#else
static char db = FALSE;
#endif

typedef enum {
    TITLE_SCREEN = 0,
    PLAYING_GAME,
    END_SCREEN,
    GAME_OVER
} State; 
static State gamestate = TITLE_SCREEN;

typedef enum {
    LEVEL_ONE,
    LEVEL_TWO,
    LEVEL_THREE,
    LEVEL_FOUR
} Level;
static Level level = LEVEL_ONE;

static int dChange = -1;
static char killFlag = FALSE;
static char spawnFlag = FALSE;

static char gameResetFlag = FALSE;

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

static char heartMoving = FALSE;
static char heartFlip = FALSE;

void handleKeys(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            if (gamestate == PLAYING_GAME) {
                Mix_ResumeMusic();
                gamestate = TITLE_SCREEN; 
                Mix_Volume(-1, 12);
                Mix_Volume(GUNSHOT, 15);
                Mix_Volume(ITEM, 24);
                Mix_Volume(STEP, 6);
            }
            else if (gamestate == TITLE_SCREEN) glfwSetWindowShouldClose(window, GLFW_TRUE);
            else if (gamestate == GAME_OVER) glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_ENTER:
            if (gamestate == TITLE_SCREEN) {
                //heartMoving = FALSE;
                Mix_PauseMusic();
                gamestate = PLAYING_GAME;
                Mix_Volume(-1, 3);
                Mix_Volume(GUNSHOT, 15);
                Mix_Volume(ITEM, 24);
                Mix_Volume(STEP, 6);
                if (tutorial) {
                    printf("\n---Press 'W' to move forward and 'S' to move back---\n");
                    printf("---Press 'A' and 'D' to look left and right---\n");
                    printf("---Press 'E' on a door to open it---\n");
                    tutorial = FALSE;
                }
            }
            else if (gamestate == GAME_OVER) {
                
                gameResetFlag = TRUE;
            }
            break;
        case GLFW_KEY_SPACE:
            heartMoving ^= TRUE;
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
            if (gamestate == TITLE_SCREEN) {
                heartFlip = TRUE;
            }
            else if (gamestate == GAME_OVER) {
                heartFlip = TRUE;
            }
            spawnFlag = TRUE;
            //buttonBuffer ^= R_DOWN;
            break;
        case GLFW_KEY_T:
            killFlag = TRUE;
            break;
        case GLFW_KEY_Y:
            db ^= TRUE;
            break;
        case COMMA:
            if (gamestate == PLAYING_GAME) {
                buttonBuffer ^= COMMA_DOWN;
                buttonBuffer ^= GUN_FIRE;
            }
            else if (gamestate == TITLE_SCREEN) {
                buttonBuffer ^= ADD_HEART;
            }
            break;
        case PERIOD:
            if (gamestate == PLAYING_GAME) { 
                buttonBuffer ^= PERIOD_DOWN; 
            }
            else if (gamestate == TITLE_SCREEN) {
                buttonBuffer ^= REMOVE_HEART;
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

void levelWipe(Sprite** s) {

    Sprite* hS = *s;

    while (hS) { // free all sprites in state 0
        Sprite* nextSprite = hS->next; // Store next before removal
        //printf("headsprite x: %f, headsprite y: %f\n", hS->x, hS->y);
        if (hS->state != 0) {
            //if (hS == headSprite) {
              //  headSprite = headSprite->next;
            //}

            spriteRemove(hS, NULL, s);
            hS = NULL;

        }

        hS = nextSprite; // Move to next sprite
    }
    killFlag = FALSE;

} // will be called when changing maps

void levelInit(Sprite ** s) {
    if (s == NULL) {
        return;
    }
    if (*s != NULL) {
        levelWipe(s);
    }
    
    Sprite* sp = *s; // create a copy of the head (basically just to avoid dereferencing every time)
    Sprite* spr; // sprite pointer for the loop
    // enemy height = -20

    // top left = 96, 96 -- add 64 per square

    if (level == LEVEL_ONE) {

        sp = newSprite(COLLECTABLE, BOTTOM_LEFT, 1, COLLECTABLE_HEALTH, 96, 96, HEART_HEIGHT); // BL heart top left

        spr = spriteAdd(sp, COLLECTABLE, BOTTOM_LEFT, 1, COLLECTABLE_HEALTH, 355, 418, HEART_HEIGHT); // BL heart bottom right

        spr = spriteAdd(spr, COLLECTABLE, TOP_LEFT, 1, COLLECTABLE_HEALTH, 96, 160, HEART_HEIGHT); // TL heart top left

        spr = spriteAdd(spr, COLLECTABLE, TOP_LEFT, 1, COLLECTABLE_HEALTH, 96, 416, HEART_HEIGHT); // TL heart bottom left

        spr = spriteAdd(spr, COLLECTABLE, TOP_RIGHT, 1, COLLECTABLE_HEALTH, 96, 416, HEART_HEIGHT); // TR heart bottom left

        spr = spriteAdd(spr, COLLECTABLE, TOP_RIGHT, 1, COLLECTABLE_HEALTH, 352, 96, HEART_HEIGHT); // TR heart top tight

        spr = spriteAdd(spr, COLLECTABLE, BOTTOM_RIGHT, 1, COLLECTABLE_HEALTH, 224, 160, HEART_HEIGHT); // BR left heart

        spr = spriteAdd(spr, COLLECTABLE, BOTTOM_RIGHT, 1, COLLECTABLE_HEALTH, 224, 288, HEART_HEIGHT); // BR middle heart

        spr = spriteAdd(spr, COLLECTABLE, BOTTOM_RIGHT, 14, GUN_HEALTH, 96, 160, HEART_HEIGHT);

        spr = spriteAdd(spr, ENEMY, TOP_LEFT, 13, ENEMY_HEALTH, 224, 288, ENEMY_HEIGHT); // TL enemy middle

        spr = spriteAdd(spr, ENEMY, TOP_RIGHT, 13, ENEMY_HEALTH, 96, 288, ENEMY_HEIGHT); // TR enemy left

    }
    // 34 = red heart
    // 46 = pink heart
    else if (level == LEVEL_TWO) {
        sp = newSprite(COLLECTABLE, BOTTOM_LEFT, 34, COLLECTABLE_HEALTH, 96, 96, HEART_HEIGHT); // BL heart top left
    }
    else if (level == LEVEL_THREE) {
        sp = newSprite(COLLECTABLE, BOTTOM_LEFT, 1, COLLECTABLE_HEALTH, 96, 96, HEART_HEIGHT); // BL heart top left
    }
    // ENEMIES ALWAYS LAST
    spawnFlag = FALSE;
    *s = sp;
}

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

    Mix_AllocateChannels(16);
    Mix_ReserveChannels(4);
    Mix_Volume(-1, 12);
    Mix_Volume(GUNSHOT, 15);
    Mix_Volume(ITEM, 24);
    Mix_Volume(STEP, 6);

    // GRAPHICS INIT
    GLFWwindow *window;
    
    if (!glfwInit())
    {
        printf("OpenGL initialization failed\n");
        glfwTerminate(); // I don't know if this function can be called here, but I can always remove it
        return -1;
    }

    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // you can't resize the window
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Steal My Heart", NULL, NULL); // name the window anything applicable 
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

    Sprite * headSprite = NULL;
    levelInit(&headSprite); 
    
    Heart* headHeart = newHeart(125,52);

    Heart* gameOverHeart = newHeart(135, 60);
    gameOverHeart->color = 0;
    Heart* gameOverHeart2 = newHeart(132, 60);
    gameOverHeart2->color = 0;

    Sprite* hS; // copies of the head for each list
    Map* hM;
    Heart* hH;

    Map * headMap = newMap(map_1_bottom_left, map_1_bottom_right, map_1_top_right, map_1_top_left);
    hM = mapAdd(headMap, map_2_bottom_left, map_2_bottom_right, map_2_top_right, map_2_top_left); // second map
    hM = mapAdd(hM, map_3_bottom_left, map_3_bottom_right, map_3_top_right, map_3_top_left); // third map
    // map is set to index 0 by default
    hM = headMap;

    while (hM) {
        for (int i = 0; i < ROOM_NUM; i++) {
            hM->c[i] = map_ceiling_small;
            hM->f[i] = map_floor_small;
        }
        hM->mapC = hM->c[0];
        hM->mapF = hM->f[0];
        hM = hM->next;
    }
    hM = headMap; // for the first time
    
    
    Player player = {
        .plX = 100,
        .plY = 400,
        .pdX = (float)cos(0.0001) * VIEWING_ANGLE_CHANGE,
        .pdY = (float)sin(0.0001) * VIEWING_ANGLE_CHANGE,
        .hasGun = FALSE, // will eventually start as false
        .moveCounter = 0
    };

    double lastTime = glfwGetTime();
    double currentTime;
    double deltaTime;
   
    double animation = 0;
    int secondAnimation = 0;
    double third_animation = 0;



    printf("Welcome to the game!!!\n\n");
    printf("---Press 'Enter' to start or press 'Esc' to pause the game---\n");
    printf("---(press 'Esc' once more in the pause screen to exit)---\n");
   
    
    // GAME LOOP

    Mix_Music* music = Mix_LoadMUS("dependencies/assets/StealmyHeart_Music.wav");
    if (!music) {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
    }
    if (Mix_PlayMusic(music, -1 ) == -1) { // -1 loops indefinitely
        printf("Failed to play music! SDL_mixer Error: %s\n", Mix_GetError());
    }
    Mix_VolumeMusic(15);


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
                hH = headHeart;

                glfwPollEvents();

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                drawSide(player);
                drawScreen(TITLE_SCREEN); // make this a variable

                // update heart
                addAHeart(hH, &buttonBuffer);
                if (buttonBuffer & REMOVE_HEART) {
                    buttonBuffer ^= REMOVE_HEART;
                    HeartRemove(NULL, &hH);
                }
                while (hH) {
                    
                    heartMove(hH, heartMoving, heartFlip);
                    drawHeart(*hH);
                    hH = hH->next;
                }
                if (heartFlip) {
                    heartFlip = FALSE;
                }
                
                glfwSwapBuffers(window); // has to be last

                break;

            case PLAYING_GAME:
                
                hS = headSprite;
                //hM = headMap;
                
                if (killFlag) {
                    levelWipe(&headSprite);
                }
                if (spawnFlag) {
                    levelInit(&headSprite);
                }

                //taking input
                glfwPollEvents();


                // "d" is a really important variable and should have a more distinct name

                int d = movePlayer(&player, deltaTime, headSprite, &buttonBuffer, hM, bTravel, &flashTimer);
                if (d > -1) {
                    dChange = d;
                    
                }
                
                if (buttonBuffer & DOOR_SLIDE) {
                    
                    if (animation == 0) {
                        if (hM->map[dChange] == 4) { // start the animation
                            hM->map[dChange] = 6;
                        }
                        else if (hM->map[dChange] == 12) {
                            hM->map[dChange] = 0; // door erased
                            
                            buttonBuffer ^= DOOR_SLIDE; 
                        }

                        if (hM->map[dChange] == 3) { // start the animation
                            hM->map[dChange] = 7;
                        }
                        else {
                            if (hM->map[dChange] != 0) {
                                if (hM->map[dChange] == 6) {
                                    hM->map[dChange] = hM->map[dChange] + 2;
                                }
                                else {
                                    hM->map[dChange] = hM->map[dChange] + 1;
                                }
                                
                            }
                        }
                    }
                }
                //printf("x: %f, y: %f\n", player.plX, player.plY);
                hS = headSprite;

                int deathTrigger = 0;

                while (hS) {

                    int a = moveSprite(hS, deltaTime, &player, *hM);
                    if (a != 0) {
                        
                        deathTrigger = a;
                    }
                        // it should be something like shootgun takes in the sprite and 
                        // runs a loop for each of the sprites, regardless of if they're NULL
                        // and bulletScan simply fires into the wall if so
                    
                    hS = hS->next;
                    
                }
                

                hS = headSprite;
                //hM = headMap;
                bulletTravel = FALSE;
                
                //rendering 
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      
                if (hM) {
                    drawRays3D(player, *hM, &bTravel, depth);
                }
                drawSide(player);
                while (hS != NULL) {
                    if (animation == 0 && hS->state == COLLECTABLE) {
                        if (hS->texture == 12) {
                            hS->texture = 1;
                        }
                        else if (hS->texture == 45) {
                            hS->texture = 34;
                        }
                        else if (hS->texture == 57) {
                            hS->texture = 46;
                        }
                        else { 
                            hS->texture += 1;
                        }

                        if (hS->texture == 33) { 
                            hS->texture = 14; 
                            hS->z = HEART_HEIGHT;
                        }
                        else if (hS->texture >= 14 && hS->texture < 33){ 
                            hS->texture += 1; // this makes it rotate faster
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
                //hM = headMap;

                if (db) {

                    drawMap2D(*hM); // gonna take in the current map
                
                    drawPlayer(player); 
                    drawBullet();

                    while (hS != NULL) {
                        drawSpriteMap(*hS);
                        hS = hS->next;
                    }

                    hS = headSprite;
                    //hM = headMap;
                    
                }
                
                
                
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

                  
                
                
                drawGun(player, buttonBuffer);
                if (animation == 0 && buttonBuffer & GUN_FIRE) {
                    secondAnimation += 1;
                    if (secondAnimation == 2) {
                        buttonBuffer ^= GUN_FIRE;
                        secondAnimation = 0;
                    }
                    
                }
                glfwSwapBuffers(window); 

                if (d == -2) { // trigger for progressing
                    if (level == LEVEL_ONE) {
                        level = LEVEL_TWO;
                        hM = hM->next;
                        player.heartCounter = 0;
                        player.plX = 100;
                        player.plY = 400;
                        levelInit(&headSprite);
                    }
                    else if (level == LEVEL_TWO) {
                        level = LEVEL_THREE;
                        hM = hM->next;
                        player.heartCounter = 0;
                        //player.plX = 100;
                        //player.plY = 400;
                        levelInit(&headSprite);
                    }
                    else if (level == LEVEL_THREE) {

                        printf("test\n");
                        gamestate = END_SCREEN;
                        
                    }
                    
                }
                else if (d == -3) { // go back a level
                    
                    if (level == LEVEL_ONE) {
                        continue;
                    }
                    else if (level == LEVEL_TWO) {
                        level = LEVEL_ONE;
                        player.heartCounter = 0;
                        hM = hM->previous;
                        hM->map = hM->m[1]; // bottom right
                        player.plX = 415;
                        player.plY = 415; 
                        levelInit(&headSprite);
                    }
                    else if (level == LEVEL_THREE) {
                        level = LEVEL_TWO;
                        player.heartCounter = 0;
                        hM = hM->previous;
                        // hM->map = hM->m[x] WHICHEVER PLACE THE DOOR IS
                        //player.plX = 400;
                        //player.plY = 400; find the real coords
                        levelInit(&headSprite);
                    }
                    
                }
                

                if (deathTrigger) {
                    
                    gamestate = GAME_OVER;
                    
                    deathTrigger = 0;
                }

                break;

            case END_SCREEN:
                glfwPollEvents();

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                drawScreen(END_SCREEN);

                glfwSwapBuffers(window);

                break;

            case GAME_OVER:
                
                glfwPollEvents();
                if (animation == 0) {
                    third_animation += deltaTime;
                }
                if (third_animation >= 0.25) {
                    if (gameOverHeart->color == 4) {
                        gameOverHeart->color = 5;
                        gameOverHeart2->color = 6;
                        heartMoving = TRUE;
                        gameOverHeart->x_direction = TRUE;
                        gameOverHeart2->x_direction = FALSE;
                    }
                    else if (gameOverHeart->color < 4) {
                        //printf("hi\n");
                        gameOverHeart->color = 4;
                    }
                    third_animation = 0;
                }
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                drawSide(player);
                drawScreen(GAME_OVER);
                //printf("color: %d\n", gameOverHeart->color);
                drawHeart(*gameOverHeart);
                Heart copy = *gameOverHeart;
                Heart copy2 = *gameOverHeart2;
                if (copy2.color == 6) { drawHeart(copy2); }
                
                if (copy.color == 5 && copy2.color == 6) {
                    heartMove(gameOverHeart, heartMoving, heartFlip);
                    heartMove(gameOverHeart2, heartMoving, heartFlip);
                }
                if (heartFlip) {
                    heartFlip = FALSE;
                }

                glfwSwapBuffers(window);
                
                if (gameResetFlag) {
                    
                    gameResetFlag = FALSE;
                    level = LEVEL_ONE;
                    levelInit(&headSprite);
                    hM = headMap;
                    hM->map = hM->m[0];
                    player.plX = 100;
                    player.plY = 400;
                    player.pa = 0.0001;
                    player.hasGun = FALSE; // will eventually start as false
                    player.moveCounter = 0;
                    player.pdX = (float)cos(0.0001) * VIEWING_ANGLE_CHANGE;
                    player.pdY = (float)sin(0.0001) * VIEWING_ANGLE_CHANGE;

                    gameOverHeart->color = 3;
                    gameOverHeart2->color = 0;
                    gameOverHeart->x_pos = 135;
                    gameOverHeart->y_pos = 60;
                    gameOverHeart2->x_pos = 132;
                    gameOverHeart2->y_pos = 60;
                    heartMoving = FALSE;

                    Mix_ResumeMusic();
                    gamestate = TITLE_SCREEN;
                }

                break;
        }

        lastTime = currentTime;
        
    }

    Mix_CloseAudio();
    SDL_Quit();
    glfwTerminate(); // get rid of the window at the end

    return 0;
}
