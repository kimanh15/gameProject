#ifndef _LOGIC__H
#define _LOGIC__H
#include <SDL.h>

void initRabbit();
void handleInput(const Uint8* keys);
void updateRabbit();
float getRabbitY();

static float rabbitY;
static float velocityY;
static bool isJumping;

const float gravity = 0.5f;
const float jumpStrength = -11.5f;
const float groundY = 380.0f;

void initRabbit() {
    rabbitY = groundY;
    velocityY = 0;
    isJumping = false;
}

void handleInput(const Uint8* keys) {
    if (keys[SDL_SCANCODE_SPACE]) {
        if (!isJumping) {
            velocityY = jumpStrength;
            isJumping = true;
        }
    }
}

void updateRabbit() {
    velocityY += gravity;
    rabbitY += velocityY;

    if (rabbitY >= groundY) {
        rabbitY = groundY;
        velocityY = 0;
        isJumping = false;
    }
}

float getRabbitY() {
    return rabbitY;
}
#endif
