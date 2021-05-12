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
UINT8 shot1X, shot1Y, shot2X, shot2Y;
UINT8 opX, opY;
UINT8 rockX, rockY;

UBYTE rockStep;
BOOLEAN rockIsMoving;
BOOLEAN shot1IsMoving, shot2IsMoving;

void play_shot_sound() {
    NR10_REG = 0x1e;
    NR11_REG = 0x50;
    NR12_REG = 0x73;
    NR13_REG = 0x10;
    NR14_REG = 0xc7;
}

void shoot(UINT8 shotNumber) {
    play_shot_sound();
    if (shotNumber == 1) {
        shot1X = shipX;
        shot1Y = shipY-4;
        set_sprite_tile(12, 26);
        move_sprite(12, shot1X, shot1Y);
        set_sprite_tile(13, 26);
        move_sprite(13, shot1X+8, shot1Y);
        shot1IsMoving = true;
    }
    if (shotNumber == 2) {
        shot2X = shipX;
        shot2Y = shipY-4;
        set_sprite_tile(14, 26);
        move_sprite(14, shot2X, shot2Y);
        set_sprite_tile(15, 26);
        move_sprite(15, shot2X+8, shot2Y);
        shot2IsMoving = true;
    }
}

void move_shots() {
    if (shot1IsMoving) {
        shot1Y -= 2;
        move_sprite(12, shot1X, shot1Y);
        move_sprite(13, shot1X+8, shot1Y);
        if (shot1Y == 0) {
            shot1IsMoving = false;
        }
    }
    if (shot2IsMoving) {
        shot2Y -= 2;
        move_sprite(14, shot2X, shot2Y);
        move_sprite(15, shot2X+8, shot2Y);
        if (shot2Y == 0) {
            shot2IsMoving = false;
        }
    }
}

void scroll_background() {
    if (step == 0)
            scroll_bkg(0, -1);

        step++;
        
        if (step == 3)
            step = 0;
}

void main() {
    DISPLAY_OFF;

    // show splashscreen
    set_bkg_data(0, 208, space_hunt_splashscreen_data);
    set_bkg_tiles(0, 0, 20, 18, space_hunt_splashscreen_map);

    SHOW_BKG;
    
    DISPLAY_ON;

    // start game when any key is pressed
    waitpad(0xFF);
    waitpadup();
    seed = DIV_REG;
    initarand(seed);

    // turn on sound
    NR52_REG = 0x80;
    NR50_REG = 0x77;
    NR51_REG = 0xff;

    // change sprite pallette (white not showing yet)
    OBP0_REG = 0x93;

    // player ship
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

    // opponent ship
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

    // background
    set_bkg_data(0, 31u, SpaceHuntBGTiles);
    set_bkg_submap(0, 0, 20, SpaceHuntMap1Height, SpaceHuntMap1, SpaceHuntMap1Width);

    SHOW_BKG;
    SHOW_SPRITES;

    DISPLAY_ON;

    step = 0;
    rockStep = 0;
    rockIsMoving = false;

    shot1IsMoving = false;
    shot2IsMoving = false;

    while(1) {
        scroll_background();

        joy = joypad();
        if (joy & J_LEFT) {
            if (shipX > 12)
                shipX--;
        } else if (joy & J_RIGHT) {
            if (shipX < 148)
                shipX++;
        }
        if (joy & J_A) {
            if (!shot1IsMoving) {
                shoot(1);
            }
            if (!shot2IsMoving && shot1Y <= 72 - (144-shipY)) {
                shoot(2);
            }
        }

        // move ship left or right
        move_sprite(0, shipX, shipY);
        move_sprite(1, shipX, shipY+8);
        move_sprite(2, shipX+8, shipY);
        move_sprite(3, shipX+8, shipY+8);

        move_shots(); // if they are to be moved

        // move opponent
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

        // move rock
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
