#include "game_types.h"

Map *newMap(int bottomleft[MAP_S], int bottomright[MAP_S], int topright[MAP_S], int topleft[MAP_S]) {
    Map *mapHead = (Map*)malloc(sizeof(Map));
    if (mapHead == NULL) {
        // allocation failed
        printf("Malloc failed!\n");
        return NULL;
    } 
    
    mapHead->previous = NULL;
    
    mapHead->m[0] = bottomleft;
    mapHead->m[1] = bottomright;
    mapHead->m[2] = topright;
    mapHead->m[3] = topleft;

    mapHead->map = mapHead->m[0];
    /*
    for (int i = 0; i < MAP_S; i++) {
        mapHead->map[i] = (mapHead->m[0])[i];
    }
    */

    mapHead->next = NULL;

    return mapHead;
}

Map *mapAdd(Map *tail, int bottomleft[MAP_S], int bottomright[MAP_S], int topright[MAP_S], int topleft[MAP_S]) {
    Map *newMap = (Map*)malloc(sizeof(Map));
    
    if (newMap == NULL) {
        return NULL;
    }

    // setting values for the item
    newMap->previous = tail;
    
    newMap->m[0] = bottomleft;
    newMap->m[1] = bottomright;
    newMap->m[2] = topright;
    newMap->m[3] = topleft;

    /*
    for (int i = 0; i < MAP_S; i++) {
        newMap->map[i] = (newMap->m[0])[i];
    }
    */
    newMap->map = newMap->m[0];

    newMap->next = tail->next;

    // updating its relation to other items
    if (tail->next != NULL) {
        tail->next->previous = newMap;
    }   

    tail->next = newMap;

    return newMap;
    // to the best of my knowledge this works with sort.c
}

void mapRemove(Map *item, int * dataCopy[ROOM_NUM]) {

    // dataCopy is an optional parameter for copying map data

    if (item == NULL) {
        return;
    }

    if (dataCopy != NULL) {
        for (int i = 0; i < ROOM_NUM; i++) {
            dataCopy[i] = item->m[i];
        }   
    }

    if (item->next != NULL) {
        item->next->previous = item->previous;
        // this works regardless of previous item status
    }
    if (item->previous != NULL) {
        item->previous->next = item->next;
        // this works regardless of previous item status
    }

    free(item);
    item = NULL;

    return;
}

void drawMap2D(Map m) { // it will take the map pointer at some point, could make it const
    int x, y, xo, yo;
    for (y = 0; y<MAP_Y;y++) { // change to y++ if it breaks
        for (x = 0; x<MAP_X; x++) {

            if (m.map[y*MAP_X+x] > 0) { // if wall tile
                glColor3f(1,1,1);
            }
            else {
                glColor3f(0,0,0);
            }
            
            xo = x*MAP_S; yo = y*MAP_S;
            glPointSize(1);
            glBegin(GL_QUADS);
            glVertex2d(xo + 1, yo + 1);
            glVertex2d(xo + 1, yo+MAP_S - 1);
            glVertex2d(xo+MAP_S - 1, yo+MAP_S - 1);
            glVertex2d(xo+MAP_S - 1, yo + 1);
            glEnd();
            

        }
    }
}

/*
drawMap3D -- does the OpenGL calls for the drawRays function, figure out shading so that this function can use it

*/