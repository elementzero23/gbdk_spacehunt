#include <gb/gb.h>
#include <rand.h>
#include <stdio.h>

#include "space_hunt_splashscreen_data.c"
#include "space_hunt_splashscreen_map.c"

#include "spacehunt_map1.c"
#include "spacehunt_bg_tiles.c"
#include "spacehunt_sprite_tiles.c"

#define true 1
#define false 0

UBYTE step;

UWORD seed;
UBYTE joy;

UINT8 shipX, shipY;
UINT8 shotX, shotY;
UINT8 opX, opY;
UINT8 rockX, rockY;

UBYTE rockStep;
BOOLEAN rockIsMoving;
BOOLEAN shotIsMoving;

void main() {
    DISPLAY_OFF;

    set_bkg_data(0, 208, space_hunt_splashscreen_data);
    set_bkg_tiles(0, 0, 20, 18, space_hunt_splashscreen_map);

    SHOW_BKG;
    
    DISPLAY_ON;

    waitpad(0xFF);
    waitpadup();
    seed = DIV_REG;
    initarand(seed);


    NR52_REG = 0x80;
    NR50_REG = 0x77;
    NR51_REG = 0xff;

    

    OBP0_REG = 0x93;

    set_sprite_data(0, 31, SpaceHuntSpriteTiles);
    set_sprite_tile(0, 6);
    set_sprite_tile(1, 4);
    set_sprite_tile(2, 7);
    set_sprite_tile(3, 5);
    shipX = 80;
    shipY = 136;
    move_sprite(0, shipX, shipY);
    move_sprite(1, shipX, shipY + 8);
    move_sprite(2, shipX + 8, shipY);
    move_sprite(3, shipX + 8, shipY + 8);

    // opponent
    set_sprite_tile(4, 8);
    set_sprite_prop(4, S_FLIPY);
    set_sprite_tile(5, 10);
    set_sprite_prop(5, S_FLIPY);
    set_sprite_tile(6, 9);
    set_sprite_prop(6, S_FLIPY);
    set_sprite_tile(7, 11);
    set_sprite_prop(7, S_FLIPY);
    do {
        opX = rand();
    } while (opX < 8 || opX > 160);
    //opX = 100;
    opY = 0;
    move_sprite(4, opX, opY);
    move_sprite(5, opX, opY+8);
    move_sprite(6, opX+8, opY);
    move_sprite(7, opX+8, opY+8);


    // rock
    set_sprite_tile(8, 16);
    set_sprite_tile(10, 18);
    set_sprite_tile(9, 17);
    set_sprite_tile(11, 19);

    move_sprite(8, 0, 0);
    move_sprite(9, 0, 0);
    move_sprite(10, 0, 0);
    move_sprite(11, 0, 0);


    set_bkg_data(0, 31u, SpaceHuntBGTiles);
    set_bkg_submap(0, 0, 20, SpaceHuntMap1Height, SpaceHuntMap1, SpaceHuntMap1Width);

    SHOW_BKG;
    SHOW_SPRITES;

    DISPLAY_ON;

    step = 0;
    rockStep = 0;
    rockIsMoving = false;

    shotIsMoving = false;

    while(1) {
        if (step == 0)
            scroll_bkg(0, -1);

        step++;
        
        if (step == 3)
            step = 0;

        joy = joypad();
        if (joy & J_LEFT) {
            if (shipX > 12)
                shipX--;
        } else if (joy & J_RIGHT) {
            if (shipX < 148)
                shipX++;
        }
        if (joy & J_A) {
            if (!shotIsMoving) {
                // play sound
            NR10_REG = 0x16;
            NR11_REG = 0x40;
            NR12_REG = 0x73;
            NR13_REG = 0x00;
            NR14_REG = 0xc3;

            shotX = shipX;
            shotY = shipY-4;
            set_sprite_tile(12, 26);
            move_sprite(12, shotX, shotY);
            set_sprite_tile(13, 26);
            move_sprite(13, shotX+8, shotY);
            shotIsMoving = true;
            }
        }

        move_sprite(0, shipX, shipY);
        move_sprite(1, shipX, shipY+8);
        move_sprite(2, shipX+8, shipY);
        move_sprite(3, shipX+8, shipY+8);

        if (shotIsMoving) {
            shotY -= 2;
            move_sprite(12, shotX, shotY);
            move_sprite(13, shotX+8, shotY);
            if (shotY == 0) {
                shotIsMoving = false;
            }
        }

        if (step > 0)
            opY++;
        if (opY == 168) {
            do {
                opY = 0;
                opX = arand();
            } while (opX < 12 || opX > 128);
        }
        move_sprite(4, opX, opY);
        move_sprite(5, opX, opY+8);
        move_sprite(6, opX+8, opY);
        move_sprite(7, opX+8, opY+8);

        
        if (!rockIsMoving) {
            if (rockStep == 4) {
                rockStep = 0;
                do {
                    rockY = 0;
                    rockX = arand();
                } while (rockX < 12 || rockX > 128);
                rockIsMoving = true;
            } else {
                rockStep++;
            }
        } else {
            move_sprite(8, rockX, rockY);
            move_sprite(9, rockX, rockY+8);
            move_sprite(10, rockX+8, rockY);
            move_sprite(11, rockX+8, rockY+8);
            rockY++;
            if (rockY == 168) {
                rockIsMoving = false;
            }
        }

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
