#include "game_types.h"

Map *newMap(int m1[MAP_S], int m2[MAP_S], int m3[MAP_S], int m4[MAP_S]) {
    Map *mapHead = (Map*)malloc(sizeof(Map));
    if (mapHead == NULL) {
        // allocation failed
        printf("Malloc failed!\n");
        return NULL;
    } 
    
    mapHead->previous = NULL;
    
    mapHead->m[0] = m1;
    mapHead->m[1] = m2;
    mapHead->m[2] = m3;
    mapHead->m[3] = m4;

    mapHead->map = mapHead->m[0];
    /*
    for (int i = 0; i < MAP_S; i++) {
        mapHead->map[i] = (mapHead->m[0])[i];
    }
    */

    mapHead->next = NULL;

    return mapHead;
}

Map *mapAdd(Map *tail, int m1[MAP_S], int m2[MAP_S], int m3[MAP_S], int m4[MAP_S]) {
    Map *newMap = (Map*)malloc(sizeof(Map));
    
    if (newMap == NULL) {
        return NULL;
    }

    // setting values for the item
    newMap->previous = tail;
    
    newMap->m[0] = m1;
    newMap->m[1] = m2;
    newMap->m[2] = m3;
    newMap->m[3] = m4;

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