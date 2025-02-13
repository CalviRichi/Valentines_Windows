#include "game_types.h"

Heart* newHeart(float x_pos, float y_pos) {
    Heart* heartHead = (Heart*)malloc(sizeof(Heart));
    if (heartHead == NULL) {
        printf("Malloc failed!\n");
        return NULL;
    }

    heartHead->previous = NULL;

    heartHead->x_pos = x_pos;
    heartHead->y_pos = y_pos;
    heartHead->color = PURPLE;

    srand(time(NULL));

    float f;
    do {
        (f = (rand() % 11) - 5);
    } while (f == 0);
    // Literally just added this cause these loops are silly
    f = 1 / f;
    
    heartHead->speed = HEART_SPEED + f;
    heartHead->x_direction = rand() % 2;
    heartHead->y_direction = rand() % 2;

    heartHead->next = NULL;

    return heartHead;
}

Heart* HeartAdd(Heart* tail) {
    Heart* newHeart = (Heart*)malloc(sizeof(Heart));

    if (newHeart == NULL) {
        printf("Malloc failed\n");
        return NULL;
    }
    srand(time(NULL));

    newHeart->previous = tail;
    newHeart->next = NULL;

    newHeart->color = rand() % 3; // there are three colors

    float f;
    do {
        (f = (rand() % 11) - 5);
    } while (f == 0);
    // Literally just added this cause these loops are silly
    f = 1 / f;

    newHeart->speed = HEART_SPEED + f;
    
    int a = HEART_X_MAX + 1 - abs(HEART_X_MIN);
    int b = HEART_Y_MAX + 1 + abs(HEART_Y_MIN);
    
   
    int c = (rand() % a);
    int d = (rand() % b);
    newHeart->x_pos = c + abs(HEART_X_MIN);
    newHeart->y_pos = d - abs(HEART_Y_MIN);
    
    newHeart->x_direction = rand() % 2;
    newHeart->y_direction = rand() % 2;

    if (tail->next != NULL) {
        //printf("%x\n", tail->next->previous);
        tail->next->previous = newHeart;
    }

    tail->next = newHeart;
    
    return newHeart;

}

void HeartRemove(Heart* sCopy, Heart** headHeart) {

    Heart* item = *headHeart;

    if (item == NULL) {
        return;
    }

    while (item->next) {
        item = item->next;
    }

    if (item == *headHeart) {
        return;
    }

    if (sCopy != NULL) {
        sCopy->color = item->color;
        sCopy->speed = item->speed;
        sCopy->x_direction = item->x_direction;
        sCopy->x_pos = item->x_pos;
        sCopy->y_direction = item->y_direction;
        sCopy->y_pos = item->y_pos;
    }

    /*
    In the functioning case, headHeart is now equal to Heart 2
    Heart 2's previous item has now been set to null
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

void heartMove(Heart* h, char heartMoving, char heartFlip) {
    
    if (heartMoving) {

        if (heartFlip) {
            h->x_direction ^= TRUE;
        }
        
        if (h->x_direction) { // moving right
            if (h->x_pos >= HEART_X_MAX) {
                h->x_direction ^= TRUE;
                playSoundEffect("dependencies/assets/heart_wall.wav", -1);
            }
            else {
                h->x_pos += HEART_SPEED;
            }
        }
        else { // moving left
            if (h->x_pos <= HEART_X_MIN) {
                h->x_direction ^= TRUE;
                playSoundEffect("dependencies/assets/heart_wall.wav", -1);
            }
            else {
                h->x_pos -= HEART_SPEED;
            }
        }

        if (h->y_direction) { // moving down
            if (h->y_pos >= HEART_Y_MAX) {
                h->y_direction ^= TRUE;
                playSoundEffect("dependencies/assets/heart_wall.wav", -1);
            }
            else {
                h->y_pos += HEART_SPEED;
            }
        }
        else { // moving up
            if (h->y_pos <= HEART_Y_MIN) {
                h->y_direction ^= TRUE;
                playSoundEffect("dependencies/assets/heart_wall.wav", -1);
            }
            else {
                h->y_pos -= HEART_SPEED;
            }
        }
    }
    
}

void addAHeart(Heart* h, static unsigned int * buttonBuffer) {
    Heart* he = h;
    if (*buttonBuffer & ADD_HEART) {
        if (he) {
            while (he->next) {
                he = he->next;
            }
        }
        HeartAdd(he);
        *buttonBuffer ^= ADD_HEART;
    }
}

