#ifndef _DEFS__H
#define _DEFS__H

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const char* WINDOW_TITLE = "Hello World!";
const char* BACKGROUND_IMG = "background.jpg";
const int redBirdTickDelay = 100;
const int rabbitTickDelay = 90;

const char*  RED_BIRD_SPRITE_FILE = "redbird.png";
const int RED_BIRD_CLIPS[][4] = {
    {0, 0, 182, 168},
    {181, 0, 182, 168},
    {364, 0, 182, 168},
    {547, 0, 182, 168},
    {728, 0, 182, 168},

    {0, 170, 182, 168},
    {181, 170, 182, 168},
    {364, 170, 182, 168},
    {547, 170, 182, 168},
    {728, 170, 182, 168},

    {0, 340, 182, 168},
    {181, 340, 182, 168},
    {364, 340, 182, 168},
    {547, 340, 182, 168},
};
const int RED_BIRD_FRAMES = sizeof(RED_BIRD_CLIPS)/sizeof(int)/4;

const char* RABBIT_SPRITE_FILE = "rabbit.png";
const int RABBIT_CLIPS[][4] = {
    {0, 0, 200, 180},
    {200, 0, 200, 180},
    {400, 0, 200, 180},
    {0, 180, 200, 180},
    {200, 180, 200, 180},
    {400, 180, 200, 180},
};
const int RABBIT_FRAMES = sizeof(RABBIT_CLIPS)/sizeof(int)/4;

#endif
