#include "render.h"
#include "dependencies/assets/textures.h"
#include "dependencies/assets/title.ppm"

void drawGun(Player p, static unsigned int bb) {
    if (p.hasGun) {
        
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 32; x++) {
                int pixel = (32 * y * 3) + (x * 3);
                int r; int g; int b;
                if (!(bb & GUN_FIRE)) {
                    r = T_PINKGUN_1[pixel];
                    g = T_PINKGUN_1[pixel + 1];
                    b = T_PINKGUN_1[pixel + 2];
                }
                else {
                    r = T_PINKGUN_2[pixel];
                    g = T_PINKGUN_2[pixel + 1];
                    b = T_PINKGUN_2[pixel + 2];
                }
                

                glPointSize(STRETCH);
                glColor3ub(r, g, b);
                glBegin(GL_POINTS);
                if (!(r==255) || !(g==0) || !(b==255)) {

                    glVertex2i(WINDOW_OFFSET + 350 + (x*7), 435 + (y*7)); // 8 not 7
                    //300
                }
                //glColor3ub(255,0,255);
                //glVertex2d(512,324);
                glEnd();
            }
        }
    }
    return;
}

void drawRays3D(Player p, Map map, int * bTravel, int depth[120]) {

    int r, mx, my, mp, dof;
    // loop counter, 
    float rx, ry, ra, xo, yo;
    float distT = 1000000;
    // rx is the rays x pos
    // xo  and yo are offset

    ra = p.pa - (30 * DR);   // change back to 30

    if (ra <= 0) {
        ra += 2 * PI;
    }
    if (ra > 2 * PI) {
        ra -= 2 * PI;
    }

     // the ray angle will fan out from 30 degrees below player to
     // 30 degrees past player angle 
    if (ra < 0) {
        ra += 2 * PI;
    }
    if (ra > 2 * PI) {
        ra -= 2 * PI;
    }

    //RAY_NUM
    for (r = 0; r < RAY_NUM; ++r) { // number of rays drawn (
    
        int vmt = 0, hmt = 0; // verticle and horizonal map texture numbers
        int cmt = 0, fmt = 0;


        // check horizontal lines
        dof = 0; // number of times we check for a surface
        float distH = 100000, hx = p.plX, hy = p.plY;

        float aTan = -1.0 / tan(ra); // negative inverse of tangent for the current ray angle 

        if (ra > PI && ra < 2 * PI) { // ray shooting downwards -- used to be ra > PI

            ry = ((((int)p.plY) / MAP_S) * MAP_S) - 0.0001; // this was originally smaller, but this value fixed it

            //  ry=(((int)py>>6)<<6) -0.0001;

            //  yo=-64; 
            //  xo=-yo*Tan;
            // round to the nearest 64th value
            rx = (p.plY - ry) * aTan + p.plX;
            //  rx=(py-ry)*Tan+px
            yo = -MAP_S; // yo and xo are y-offset and x-offset
            xo = -yo * aTan;
        }
        else if (ra < PI && ra > 0) { // looking up     
            ry = ((((int)p.plY) / MAP_S) * MAP_S) + MAP_S;
            //ry=(((int)py>>6)<<6)+64;
            rx = (p.plY - ry) * aTan + p.plX;
            //rx=(py-ry)*Tan+px; 
            yo = MAP_S; // yo and xo are y-offset and x-offset
            xo = -yo * aTan;
        }
        else { // looking straight left or right
            rx = p.plX;
            ry = p.plY;
            dof = DOF;
        }

        // this is still for the horizontal lines
        while (dof < DOF) {
          
            mx = ((int)rx) / MAP_S;
            my = ((int)ry) / MAP_S;
            mp = (my * MAP_X) + mx; // my*mapX will set your row, mx will move a number into the row

            if (mp >= 0 && mp < MAP_X * MAP_Y && map.map[mp] > 0) { // add in a "mp > 0"

                // if the ray's coordinates are inside of a wall

                hmt = map.map[mp];

                dof = DOF; // hit wall

                hx = rx;
                hy = ry;
                distH = dist(p.plX, p.plY, hx, hy, ra);
            }
            else {
                // scan for the next sector
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }

        /*

        ray drawing

        glColor3f(0,1,0);
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2d(plX, plY);
        glVertex2d(rx, ry);
        glEnd();
        */
        // for drawing both lines

        // end check horiz


        // check verticle lines
        dof = 0;
        // int side = 0;
        float distV = 100000, vx = p.plX, vy = p.plY;
        float Tan = -tan(ra);
        if (ra > P2 && ra < P3) { // looking left -- ra > P2 && ra < P3)
            rx = (((int)p.plX / MAP_S) * MAP_S) - 0.0001;
            ry = (p.plX - rx) * Tan + p.plY;
            xo = -MAP_S; // yo and xo are y-offset and x-offset
            yo = -xo * Tan;
        }
        else if (ra < P2 || ra > P3) { // looking right

            rx = (((int)p.plX / MAP_S) * MAP_S) + MAP_S;

            ry = (p.plX - rx) * Tan + p.plY;
            xo = MAP_S; // yo and xo are y-offset and x-offset
            yo = -xo * Tan;
        }
        else { // looking straight up or down
            ry = p.plY;
            rx = p.plX;
            dof = DOF;
        }
        while (dof < DOF) {
            
            mx = (int)(rx) / MAP_S;
            my = (int)(ry) / MAP_S;
            mp = my * MAP_X + mx;
            //mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
            if (mp >= 0 && mp < MAP_X * MAP_Y && map.map[mp]>0) {
                // needs to be within the bounds of map
                vmt = map.map[mp];

             
                dof = DOF; // hit wall

                vx = rx;
                vy = ry;
                
                distV = dist(p.plX, p.plY, vx, vy, ra); // original
                
                //distV=cos(ra)*(rx-p.plX)-sin(ra)*(ry-p.plY);
            }
            else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }
        
        char drawColor = 0;
        if (distV < distH) {
            rx = vx;
            ry = vy;
            hmt = vmt;
            // just to reuse a variable
            distT = distV;
            
            drawColor = 1;
            glColor3f(1, 0, 0);

        }
        
        else if (distH < distV) {
            rx = hx;
            ry = hy;

            distT = distH;
            
            drawColor = 0;
            glColor3f(0.8, 0, 0);
        }
        
       
        int* tile;
        switch (hmt) {

        case 1:
            tile = T_BRICKS_2;
            break;
        case 4:
            tile = T_DOOR1;
            break;
        case 5:
            tile = T_HALLWAY;
            break;
        case 6:
            tile = T_DOOR1;
            break;
        case 7:
            tile = T_DOOR2;
            break;
        case 8:
            tile = T_DOOR3;
            break;
        case 9:
            tile = T_DOOR4;
            break;
        case 10:
            tile = T_DOOR5;
            break;
        case 11:
            tile = T_DOOR6;
            break;
        default:
            tile = T_MALANE2;
            break;
        }

        

        // color thing used to be here


        /*
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2d(plX, plY);
        glVertex2d(rx, ry);
        glEnd();
        */

        // end check vert

        // draw 3d component 

        float ca = p.pa - ra;

        // player angle minus ray angle

        if (ca < 0) {
            ca += 2 * PI;
        }
        if (ca > 2 * PI) {
            ca -= 2 * PI;
        }
        
        
        distT = distT * cos(ca); // fix the fisheye effect
        
        // as ray angle moves away from player viewing angle, shorten
        // the lines to not create a fisheye

        if (r == 60) {
            *bTravel = (int)distT;
        }

        int lineH, lineO; // declared

        // if this breaks, get rid of the conditional

        lineH = (MAP_S * SCREEN_HEIGHT) / distT; // lines get taller the closer you are. 320 keeps everything cubic

      
        float ty_step = 32.0 / (float)lineH;
        float ty_off = 0;
        if (lineH > SCREEN_HEIGHT) {
            ty_off = (lineH - SCREEN_HEIGHT) / 2;
            lineH = SCREEN_HEIGHT;
        }

        lineO = (SCREEN_HEIGHT / 2) - lineH / 2; // this fixes their position on the screen
        
        if (lineO < 0) {
            lineO = 0;
        }
        
        depth[r] = distT;
      
        

         // do color change before drawing main lines

        if (drawColor) {
            glColor3f(0.9, 0, 0);
        }
        else {
            glColor3f(0.7, 0, 0);
        }

        /*
        glLineWidth(7);
        glBegin(GL_LINES);
        glVertex2i(r*7+530,lineO); // 530 is to offset it onto the other side of the screen
        glVertex2i(r*7+530,lineH+lineO);
        glEnd();
        */

        int y;
        // fix something here
        float ty = ty_off * ty_step; // texture's y values
        float tx;
        if (!drawColor) {
            tx = (int)(rx / 2.0) % 32;
            if (!(ra > PI)) {
                tx = 31 - tx;
            }
        }
        else {
            tx = (int)(ry / 2.0) % 32;
            if (ra > P2 && ra < P3) {
                tx = 31 - tx;
            }
        }
        
        // walls

        for (y = 0; y < lineH; y++) {

            int pixel = ((int)ty * 32 + (int)tx) * 3;
            int red = tile[pixel];// swap to T_GRASS if you want 
            int green = tile[pixel + 1];
            int blue = tile[pixel + 2];
            glPointSize(STRETCH);

            if (hmt == 4 || hmt > 5) {
                if (!(red == 255) || !(green == 255) || !(blue == 255)) {
                    glColor3ub(red, green, blue);
                    glBegin(GL_POINTS);
                    glVertex2i(r * STRETCH + WINDOW_OFFSET + SHIFT, y + lineO);
                    glEnd();
                }
            }
            else {
                glColor3ub(red, green, blue);
                glBegin(GL_POINTS);
                glVertex2i(r * STRETCH + WINDOW_OFFSET + SHIFT, y + lineO);
                glEnd();
            }

            ty += ty_step;
        }
        
        // floor
       
        for (int x = lineO + lineH; x < SCREEN_HEIGHT; x++) {

            float dx = x - (SCREEN_HEIGHT / 2.0); // 512 is equal to screen height 
            // ca is equal to player angle minus ray angle 
            // ra is ray angle
            // plX is player X pos
            // plY is player y pos
            
            tx = (p.plX / 2) + (cos(ra) * (SCREEN_HEIGHT/2) * 32 / dx / cos(ca)); // 255
            ty = (p.plY / 2) + (sin(ra) * (SCREEN_HEIGHT/2) * 32 / dx / cos(ca));
            
            mp = ((int)(ty / 32.0) * MAP_X + (int)(tx / 32.0));
          
            int pixel = (((int)(ty) & 31) * 32 + ((int)(tx) & 31)) * 3;
            
            /*
            eventually use a floor and ceiling array + number checker
            to set different tiles and such
            */
            //int mf = mapF[(int)(ty/32.0)*mapX+(int)(tx/32.0)]*32*32;
            //int mf = mapF[mp];
            if (mp >= 0 && mp <= MAP_S) {
                fmt = map.mapF[mp];
            }
            int* floorTile;
            //fmt = hmt;
            switch (fmt) {
            case 1:
                floorTile = T_LEIGHA;
                break;

            case 2:
                floorTile = T_GRASS_2;
                break;

            default:
                floorTile = T_MALANE2;
                break;
            }

            //int mc = mapC[(int)(ty/32.0)*mapX+(int)(tx/32.0)]*32*32;
            //int mc = mapC[mp];
            if (mp >= 0 && mp <= MAP_S) {
                cmt = map.mapC[mp];
            }
            int* ceilingTile;
            //cmt = hmt;
            switch (cmt) {
            case 1:
                ceilingTile = T_WOOD_PLANKS;
                break;
            case 2:
                ceilingTile = T_MALANE2;
                break;
            default:
                ceilingTile = T_BRICKS_2;
                break;
            }
        


            int red = floorTile[pixel];
            int green = floorTile[pixel + 1];
            int blue = floorTile[pixel + 2];
            glPointSize(STRETCH);
            glColor3ub(red, green, blue);
            glBegin(GL_POINTS);
            glVertex2i(r * STRETCH + WINDOW_OFFSET + SHIFT, x);
            glEnd();

            // ceilings

            int pixelC = (((int)(ty) & 31) * 32 + ((int)(tx) & 31)) * 3;
            unsigned int redC = ceilingTile[pixelC];
            unsigned int greenC = ceilingTile[pixelC + 1];
            unsigned int blueC = ceilingTile[pixelC + 2];
            glPointSize(STRETCH);
            glColor3ub(redC, greenC, blueC);
            glBegin(GL_POINTS);
            glVertex2i(r * STRETCH + WINDOW_OFFSET+SHIFT, SCREEN_HEIGHT - x);
            glEnd();

        


        }

        
        
        ra += (DR / 2.0); // add one degree to the ray angle 
        if (ra <= 0) {
            ra += 2 * PI;
        }
        if (ra > 2 * PI) {
            ra -= 2 * PI;
        }
      
    }
    
    return;
}

void drawSprite(Sprite* sp, Player p, Map m, int* flashTimer, int dt, int depth[120]) {

    float scalingFactorX = (int)SCALE_X / (2 * tan(60 * DR / 2));
    float scalingFactorY = (int)SCALE_Y / (2 * tan(60 * DR / 2));

    if (sp->state == OFF || m.map != m.m[sp->region]) {

        if (*flashTimer == 0 || m.map != m.m[sp->region]) {
            //spriteRemove(&sp, NULL); // this causes a crash
            // memory is being freed, but sp is not set to null
            return;
        }
    }
   
    Sprite s = *sp;

    float spriteX = s.x - p.plX; // Temp variables for sprite coordinates
    float spriteY = s.y - p.plY;
    float spriteZ = s.z;

    float cosAngle = cos(p.pa);
    float sinAngle = sin(p.pa);

    float transformedX = spriteY * cosAngle - spriteX * sinAngle;
    float transformedY = spriteX * cosAngle + spriteY * sinAngle;
    float transformedZ = spriteZ;

    if (transformedY <= 0) return;
    if (transformedY < 0.3f) transformedY = 0.3f;

    float distanceToSprite = sqrt(spriteX * spriteX + spriteY * spriteY);

    // investigate this part so that sprites display more accurately
    float screenX = (transformedX * scalingFactorX / transformedY) + ((int)SCALE_X / 2); // number of rays

    // float screenY = ((transformedZ * (scalingFactorY / transformedY)) + ((int)SCALE_Y / 2)) - (distanceToSprite / 64); // -distanceToSprite/64
    int scale = (32 * SCALE_Y) / transformedY;
    if (scale < 0) {
        scale = 0;
    }
    if (scale > 120) {
        scale = 120; // arbitrary
    }

    float screenY = (SCALE_Y / 2) - (transformedZ * scalingFactorY / transformedY) + scale / 2;

    if (distanceToSprite > 216) {
        screenY += (distanceToSprite - 216) / 64;
    }

    // this is an estimate

    int x, y;


    float t_x = 0, t_y = 31, t_x_step = 31.5 / (float)scale, t_y_step = 32.0 / (float)scale;

    for (x = screenX - scale / 2; x < screenX + scale / 2; x++) {
        t_y = 31;
        for (y = 0; y < scale; y++) {

            if (x > 0 && x < 120 && transformedY < depth[x]) {
                // this makes the sprite draw out of bounds but will be fixed when the screen size issue is fixed
                int pixel = ((int)t_y * 32 + (int)t_x) * 3;

                int* t;

                switch (s.texture) {
                case 1:
                    t = T_HEART_1;
                    break;
                case 2:
                    t = T_HEART_2;
                    break;
                case 3:
                    t = T_HEART_3;
                    break;
                case 4:
                    t = T_HEART_4;
                    break;
                case 5:
                    t = T_HEART_5;
                    break;
                case 6:
                    t = T_HEART_6;
                    break;
                case 7:
                    t = T_HEART_7;
                    break;
                case 8:
                    t = T_HEART_8;
                    break;
                case 9:
                    t = T_HEART_9;
                    break;
                case 10:
                    t = T_HEART_10;
                    break;
                case 11:
                    t = T_HEART_11;
                    break;
                case 12:
                    t = T_HEART_1;
                    break;
                case 13:
                    t = T_CUPID;
                    break;
                case 14:
                    t = T_GUNITEM_1;
                    break;
                case 15:
                    t = T_GUNITEM_2;
                    break;
                case 16:
                    t = T_GUNITEM_3;
                    break;
                case 17:
                    t = T_GUNITEM_4;
                    break;
                case 18:
                    t = T_GUNITEM_5;
                    break;
                case 19:
                    t = T_GUNITEM_6;
                    break;
                case 20:
                    t = T_GUNITEM_7;
                    break;
                case 21:
                    t = T_GUNITEM_8;
                    break;
                case 22:
                    t = T_GUNITEM_9;
                    break;
                case 23:
                    t = T_GUNITEM_10;
                    break;
                case 24:
                    t = T_GUNITEM_11;
                    break;
                case 25:
                    t = T_GUNITEM_12;
                    break;
                case 26:
                    t = T_GUNITEM_13;
                    break;
                case 27:
                    t = T_GUNITEM_14;
                    break;
                case 28:
                    t = T_GUNITEM_15;
                    break;
                case 29:
                    t = T_GUNITEM_16;
                    break;
                case 30:
                    t = T_GUNITEM_17;
                    break;
                case 31:
                    t = T_GUNITEM_18;
                    break;
                case 32:
                    t = T_GUNITEM_19;
                    break;
                case 33:
                    t = T_GUNITEM_20;
                    break;
                default:
                    t = T_CUPID;
                    break;
                }

                int r = t[pixel];
                int g = t[pixel + 1];
                int b = t[pixel + 2];


                if (*flashTimer > 0) {
                    *flashTimer -= 1;
                    if (*flashTimer <= 0) *flashTimer = 0;
                    //printf("Timer: %d\n", *flashTimer);
                    if (s.state == ENEMY || s.state == OFF) r = 255;
                }
                
                if (!(r == 255) || !(g == 0) || !(b == 255)) { // this means don't draw
                    if (!(r == 254) && !(g == 254) && !(b == 254)){
                        glPointSize(STRETCH);
                        glColor3ub(r, g, b);
                        glBegin(GL_POINTS);
                        glVertex2d((x * STRETCH) + WINDOW_OFFSET + SHIFT, screenY * STRETCH - y * STRETCH);
                        glVertex2d((x * STRETCH) + WINDOW_OFFSET + SHIFT, screenY * STRETCH - y * STRETCH);
                        glEnd();
                    }
                }

                t_y -= t_y_step;
                if (t_y < 0) {
                    t_y = 0;
                }
            }

        }
        t_x += t_x_step;

    }

    return;
}

void drawScreen(int v) {
    int x; int y;
    int* T;

    switch (v) {
    case 1:
        T = S_TITLE;
        break;
    default:
        T = S_TITLE;
        break;
    }
    
    for (y = 0; y < 80; y++) {
        for (x = 0; x < 120; x++) {
            int pixel = ((y * 120) + x) * 3;
            int red = T[pixel + 0];
            int green = T[pixel + 1];
            int blue = T[pixel + 2];

            glPointSize(STRETCH);
            glColor3ub(red, green, blue);
            glBegin(GL_POINTS);
            glVertex2d(SHIFT + WINDOW_OFFSET + (x * STRETCH), y * STRETCH);
            glEnd();
        }
    }

}

void drawSide(Player p) {
    int x; int y;
    int* T = T_HEART_1;

    for (y = 0; y < SCREEN_HEIGHT; y++) {
        for (x = 0; x < SIDE_ADD; x++) {
            glPointSize(STRETCH);
            glColor3ub(30, 30, 30);
            glBegin(GL_POINTS);
            glVertex2d((x + (SCREEN_WIDTH - SIDE_ADD)), y);
            glVertex2d(x, y);
            glEnd();
        }
    }
    if (p.heartCounter > 0) {
        int offsetX = 0;
        int offsetY = 0;
        for (int i = 0; i < p.heartCounter; i++) {
            if (i >= 5) {
                offsetX = 352;
                offsetY = i - 5;
            }
            else {
                offsetX = 0;
                offsetY = i;
            }
            for (y = 0; y < 32; y++) {
                for (x = 0; x < 32; x++) {
                    int pixel = (32 * y * 3) + (x * 3);
                    int r = T[pixel]; int g = T[pixel + 1]; int b = T[pixel + 2];
                    if (!(r > 200 && g > 200)) {
                        glPointSize(STRETCH);
                        glColor3ub(r, g, b);
                        glBegin(GL_POINTS);
                        glVertex2d((offsetX + 2 + x) * 3, (32*offsetY + 5 +  y) * 3);
                        glEnd();
                    }

                }
            }
        }
    }
    
}