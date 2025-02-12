#include "game_types.h"

static float interact = 0;
#define ATTACK_RATE 9

Sprite *newSprite(SPRITE_TYPE state, int region, int texture, int health, float x, float y, float z) {
    Sprite *spriteHead = (Sprite*)malloc(sizeof(Sprite));
    if (spriteHead == NULL) {
        printf("Malloc failed!\n");
        return NULL;
    }

    spriteHead->previous = NULL;

    spriteHead->state = state;
    spriteHead->region = region;
    spriteHead->texture = texture;
    spriteHead->health = health;
    spriteHead->x = x;
    spriteHead->y = y;
    spriteHead->z = z;

    spriteHead->next = NULL;

    return spriteHead;
}

Sprite *spriteAdd(Sprite * tail, SPRITE_TYPE state, int region, int texture, int health, float x, float y, float z) {
    Sprite * newSprite = (Sprite*)malloc(sizeof(Sprite));

    if (newSprite == NULL) {
        printf("Malloc failed\n");
        return NULL;
    }

    newSprite->previous = tail;
    newSprite->next = NULL;

    newSprite->state = state;
    newSprite->region = region;
    newSprite->texture = texture;
    newSprite->health = health;
    newSprite->x = x;
    newSprite->y = y;
    newSprite->z = z;

    if (tail->next != NULL) {
        //printf("%x\n", tail->next->previous);
        tail->next->previous = newSprite;
    }

    tail->next = newSprite;

    return newSprite;

}

void spriteRemove(Sprite * item, Sprite * sCopy, Sprite ** headSprite) {

    if (item == NULL) {
        return;
    }

    if (item == *headSprite) {
        *headSprite = item->next;
    }

    if (sCopy != NULL) {
        sCopy->health = item->health;
        sCopy->state = item->state;
        sCopy->texture = item->texture;
        sCopy->region = item->region;
        sCopy->x = item->x;
        sCopy->y = item->y;
        sCopy->z = item->z;
    }

    /*
    In the functioning case, headsprite is now equal to sprite 2
    sprite 2's previous item has now been set to null
    */

    if (item->next) {
        item->next->previous = item->previous;
    }
    if (item->previous) {
        item->previous->next = item->next;
    }

    free(item);

    return;
}

void drawSpriteMap(Sprite s) {
    
    glPointSize(8);
    glBegin(GL_POINTS);
    glColor3ub(0,255,0);
    glVertex2d(s.x, s.y);
    glEnd();
}

// different functions for different sprites
void moveSprite(Sprite * s, float dt, Player * pl, Map m) {
    
    Player p = *pl;
    if (interact < ATTACK_RATE) {
        interact += dt;
    }

    if (s->state == OFF || m.map!=m.m[s->region]) { // m.map!=m.m[s->region]
        return;
    }

    if (s->state == ENEMY) { // if it is an enemy

    int spx = ((int)s->x)>>6, spy = ((int)s->y)>>6; // bitshifting right by 6 is the same as dividing by 64
    int spx_add = ((int)s->x+5)>>6, spy_add = ((int)s->y+5)>>6;
    int spx_sub = ((int)s->x-5)>>6, spy_sub = ((int)s->y-5)>>6;
    //printf("hello\n");

    int boost = 1;

    if (m.map[spy*MAP_X+spx_sub] == 0) {
        //noLeft = FALSE;
    }
    else {
        //noLeft = TRUE;
    }


    //if (noLeft) {

    //}
    if (s->x>p.plX && m.map[spy*MAP_X+spx_sub] == 0) {
        s->x-=SPRITE_SPEED*dt;
    }
    if (s->x<p.plX && m.map[spy*MAP_X+spx_add] == 0) {
        s->x+=SPRITE_SPEED*dt;
        //printf("alsoTRUE\n");
        
    }
    if (s->y>p.plY && m.map[spy_sub*MAP_X+spx] == 0) {
        s->y-=SPRITE_SPEED*dt;
        
    }
    if (s->y<p.plY && m.map[spy_add*MAP_X+spx] == 0) {
        s->y+=SPRITE_SPEED*dt;
    }
    
    float d = dist(s->x, s->y, p.plX, p.plY, 0);
    if (d < 10 && interact > ATTACK_RATE) {
        if (pl->heartCounter > 0) {
            pl->heartCounter--;
        }
        interact = 0;
    }

    return;

    }

    if (s->state == COLLECTABLE) {
        /*
        Code for detecting when the player has intersected with the sprite
        Could include a sound effect
        Need to add +1 to the score 
        */
        float x1 = s->x - p.plX; float y1 = s->y - p.plY;
        float d = sqrt(x1*x1 + y1*y1);
        if (d < 10 && s->health == COLLECTABLE_HEALTH && pl->heartCounter < 9) {
            s->state = OFF;
           
            if (pl->heartCounter < 9) { 
                pl->heartCounter++;
                playSoundEffect("dependencies/assets/coin.wav", ITEM);
            }
            
        }
        else if (d < 10 && s->health == (GUN_HEALTH)) {
         
            s->state = OFF;
            pl->hasGun = TRUE;
            playSoundEffect("dependencies/assets/coin.wav", ITEM);
        }
       
        return;
    }
}

/*
Should have a situation where
*/