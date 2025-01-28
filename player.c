#include "game_types.h"

static int b_x, b_x_add, b_x_sub, b_y, b_y_add, b_y_sub;
static int beginPosX, beginPosY, endPosX, endPosY;

void shootGun(Sprite * s, Player * p, int bTravel, int * flashTimer) {
    if (p->hasGun) {
        PlaySound(TEXT("dependencies/assets/gunshot.wav"), NULL, SND_FILENAME | SND_ASYNC);
        while (1) {
            bulletScan(s, p, bTravel, flashTimer);
            //printf("value = %d\n", bTravel);
            if (s != NULL && s->next != NULL) {
                s = s->next;
            }
            else {
                break;
            }
        }
    }
    
}

void drawBullet() {
    
    glPointSize(1);
    glBegin(GL_LINES);
    glColor3ub(255,0,255);
    glVertex2d(beginPosX, beginPosY);
    glVertex2d(endPosX, endPosY);
    glEnd();

    glPointSize(4);
    glBegin(GL_POINTS);
    glColor3ub(255,0,0);
    glVertex2d(b_x_sub, b_y_sub);
    glVertex2d(b_x_sub, b_y_add);
    glVertex2d(b_x_add, b_y_add);
    glVertex2d(b_x_add, b_y_sub);
    glEnd();
}

void bulletScan(Sprite * s, Player * pl, int bTravel, int * flashTimer) {
    //printf("pdx: %f\npdy: %f\n", pdX, pdY);
    Player p = *pl;
    
    float px = fabs(p.pdX), py = fabs(p.pdY);
    float mag = (int)sqrt((px*px) + (py*py));
    for (int i = 0; i < bTravel/mag; i++) {
        b_x = (int)(p.plX+p.pdX*i);
        b_y = (int)(p.plY+p.pdY*i);
        if (i == 1) {
            beginPosX = b_x; beginPosY = b_y;
        }
        
        b_x_add = b_x+8; b_x_sub = b_x-8;
        b_y_add = b_y+8; b_y_sub = b_y-8;
        
        if (s != NULL && s->state == 1 && s->type == 1) {
        if ((s->x < b_x_add && s->x > b_x_sub) && (s->y < b_y_add && s->y > b_y_sub)) {
            s->health--;
            //printf("sprite health: %d\n", s->health);
            // 1250 works good too
            int div = (i>>3) ? (i>>3) : 1;
            // this really should take in the frame rate instead
            if (s->health <= 0 && s->state == 1) {
                s->state = 0; // turn the sprite off
                pl->heartCounter++; // give the player one heart

                // Play an enemy death sound

               
                *flashTimer += 2000/div;
              
            }
            else if (s->health > 0) {
                *flashTimer += 2000/div;
                //printf("hello\n");
            }
            endPosX = b_x; endPosY = b_y;
            return;
        }
        }
        
    }
    endPosX = b_x; endPosY = b_y;
    //printf("begin x: %d, begin y: %d\nend x: %d, end y: %d\n", beginPosX, beginPosY, endPosX, endPosY);
}

void movePlayer(Player * p, float deltaTime, Sprite * s, unsigned char * buttonBuffer, Map * m, int bTravel, int * flashTimer)
{
    int xo;     // position for collision detection
    if (p->pdX<0) {
        xo = -MIN_DISTANCE; // if we're going backwards it should be behind us
        // the value of 20 can be changed to whatever makes sense
    }
    else {
        xo = MIN_DISTANCE;
    }
    int yo;
    if (p->pdY<0) {
        yo = -MIN_DISTANCE;
    }
    else {
        yo = MIN_DISTANCE;
    }

    int ipx = p->plX/MAP_S; // player's grid X position
    int ipx_add_xo = (p->plX+xo)/MAP_S;
    int ipx_sub_xo = (p->plX-xo)/MAP_S;
    int ipy = p->plY/MAP_S;
    int ipy_add_yo = (p->plY+yo)/MAP_S;
    int ipy_sub_yo = (p->plY-yo)/MAP_S;
    //printf("pdx: %f, pdy: %f\n", pdX, pdY);
    //printf("plx: %f, ply: %f\n", plX, plY);
    if (*buttonBuffer & W_DOWN)
    {
        
        if (m->map[ipy*MAP_X + ipx_add_xo]==0) { // if the sector in front of you is empty you can move
            //printf("Hello\n");
            p->plX += p->pdX*deltaTime*PLAYER_SPEED;
        }
        
        // these being separated causes the sliding on the wall effect
        if (m->map[ipy_add_yo*MAP_X + ipx] == 0) {
            p->plY += p->pdY*deltaTime*PLAYER_SPEED;
        }
        
        if (m->map[ipy*MAP_X + ipx_add_xo]==5 || m->map[ipy_add_yo*MAP_X + ipx] == 5) {
            
            if (p->pdX > 4.4) { // approximate value
                if (p->plX - 320 > 90) {
                    p->plX-=335;
                }
                else {
                    return;
                }
                
                if (m->map == m->m[0]) {
                    m->map = m->m[1];
                    //printf("hi\n");
                }
                else if (m->map == m->m[3]) {
                    m->map = m->m[2];
                }
                
            }
            else if (p->pdX < -4.4) {
                if (p->plX + 320 < 420) {
                    p->plX+=335;
                }
                else {
                    return;
                }
                //printf("hello\n");
                if (m->map == m->m[1]) {
                    m->map = m->m[0];
                }
                else if (m->map == m->m[2]) {
                    m->map = m->m[3];
                }
                
            }
            else if (p->pdY > 4.4) {
                if (p->plY - 320 > 90) {
                    p->plY-=335;
                }
                else {
                    return;
                }
                //printf("bye\n");
                if (m->map == m->m[3]) {
                    m->map = m->m[0];
                }
                else if (m->map == m->m[2]) {
                    m->map = m->m[1];
                }
            }
            else if (p->pdY < -4.4) {
                if (p->plY + 320 < 420) {
                    p->plY+=335;
                }
                else {
                    return;
                }
                //printf("goodbye\n");
                if (m->map == m->m[0]) {
                    m->map = m->m[3];
                }
                else if (m->map == m->m[1]) {
                    m->map = m->m[2];
                }

            }
        }
    }
    if (*buttonBuffer & A_DOWN)
    {
        p->pa-=(deltaTime*(VIEWING_ANGLE_CHANGE-3.2)); // change to -
        if (p->pa<0) {
            p->pa+=2*PI;
        }
        p->pdX = cos(p->pa)*VIEWING_ANGLE_CHANGE; // change this value to what feels right
        p->pdY = sin(p->pa)*VIEWING_ANGLE_CHANGE;
    }
    if (*buttonBuffer & S_DOWN)
    {
        if (m->map[ipy*MAP_X + ipx_sub_xo]==0) {
            p->plX -= p->pdX*deltaTime*PLAYER_SPEED;
        }
        if (m->map[ipy_sub_yo*MAP_X + ipx] == 0) {
            p->plY -= p->pdY*deltaTime*PLAYER_SPEED;
        }

        if (m->map[ipy*MAP_X + ipx_sub_xo]==5 || m->map[ipy_sub_yo*MAP_X + ipx] == 5) {
            
            if (p->pdX < -4.4) { // approximate value
                if (p->plX - 320 > 90) {
                    p->plX-=335;
                }
                else {
                    return;
                }
                //printf("hi\n");
                if (m->map == m->m[0]) {
                    m->map = m->m[1];
                }
                else if (m->map == m->m[3]) {
                    m->map = m->m[2];
                }
                
            }
            else if (p->pdX > 4.4) {
                if (p->plX + 320 < 420) {
                    p->plX+=335;
                }
                else {
                    return;
                }
                //printf("hello\n");
                if (m->map == m->m[1]) {
                    m->map = m->m[0];
                }
                else if (m->map == m->m[2]) {
                    m->map = m->m[3];
                }
                
            }
            else if (p->pdY < -4.4) {
                if (p->plY - 320 > 90) {
                    p->plY-=335;
                }
                else {
                    return;
                }
                //printf("bye\n");
                if (m->map == m->m[3]) {
                    m->map = m->m[0];
                }
                else if (m->map == m->m[2]) {
                    m->map = m->m[1];
                }
            }
            else if (p->pdY > 4.4) {
                if (p->plY + 320 < 420) {
                    p->plY+=335;
                }
                else {
                    return;
                }
                
                //printf("goodbye\n");
                if (m->map == m->m[0]) {
                    m->map = m->m[3];
                }
                else if (m->map == m->m[1]) {
                    m->map = m->m[2];
                }

            }
        }
        
    }
    if (*buttonBuffer & D_DOWN)
    {
        p->pa+=(deltaTime*(VIEWING_ANGLE_CHANGE-3.2)); // change to +
        if (p->pa>2*PI) {
            p->pa-=2*PI;
        }
        p->pdX = cos(p->pa)*VIEWING_ANGLE_CHANGE; // change this value to what feels right
        p->pdY = sin(p->pa)*VIEWING_ANGLE_CHANGE;
    }
    
    if (*buttonBuffer & E_DOWN) {
        
        if (xo < 0) {
            xo-=16;
        }
        else {
            xo+=16;
        }
        if (yo < 0) {
            yo-=16;
        }
        else {
            yo+=16;
        }
        
        if (m->map[ipy_add_yo*MAP_X+ipx_add_xo] == 4) {
            if (p->heartCounter > 1) {
                m->map[ipy_add_yo * MAP_X + ipx_add_xo] = 0;
            }
            else {
                printf("\n-You do not have enough hearts to open this door!-\n");
            }
        }
        *buttonBuffer ^= E_DOWN;
        

    }

    if (*buttonBuffer & COMMA_DOWN) {
        *buttonBuffer ^= COMMA_DOWN;
        shootGun(s, p, bTravel, flashTimer);// Sprite * s, Player p, int bTravel, int * flashTimer
    }
    else if (*buttonBuffer & R_DOWN) {
    // when sprites are a linked list, the head will be passed as an argument
    *buttonBuffer ^= R_DOWN;
    
    while (s) {
        //printf("hi\n");
        if (s->type == 1) {
            s->state = 1;
            s->health = 3; // this can't exactly be it, different sprites have different health
        }
        else if (s->type == 2) {
            // CHANGE THIS LATER   
            s->state = 1;
            s->health = 3;
        }
        s = s->next;
    }
    
    
    }

    //printf("player x: %d, player y: %d, %f\n", xo, yo, pa);
    //printf("pa = %f\n",pa);
    
}

void drawPlayer(Player p)
{


    glColor3f(1, 1, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2d(p.plX, p.plY);
    glEnd();
    
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2d(p.plX,p.plY);
    glVertex2d(p.plX+p.pdX*5, p.plY+p.pdY*5); // again 5 can be changed
    //printf("player x: %f, player y: %f\nplayer x dir: %f, player y dir: %f\n", plX, plY, pdX, pdY);
    glEnd();
    
}