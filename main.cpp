#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>

#include "defs.h"
#include "graphics.h"
#include "logic.h"
#include "audio.h"

using namespace std;

bool waitingForKeyRelease = false;

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_PollEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

int main(int argc, char* argv[])
{
    Graphics graphics;
    graphics.init();

    Audio audio;
    audio.loadAudio();
    audio.playBackgroundMusic();

    ScrollingBackground background;
    background.setTexture(graphics.loadTexture(BACKGROUND_IMG));

    Sprite redBird;
    SDL_Texture* redBirdTexture = graphics.loadTexture(RED_BIRD_SPRITE_FILE);
    redBird.init(redBirdTexture, RED_BIRD_FRAMES, RED_BIRD_CLIPS);

    Sprite rabbit;
    SDL_Texture* rabbitTexture = graphics.loadTexture(RABBIT_SPRITE_FILE);
    rabbit.init(rabbitTexture, RABBIT_FRAMES, RABBIT_CLIPS);

    obstacleManager.loadTextures(graphics);

    SDL_Texture* notificationBoard = graphics.loadTexture(NOTIFICATION_BOARD_IMG);

    int redBirdTickCounter = 0;
    int rabbitTickCounter = 0;


    initRabbit();

    bool quit = false;
    bool isGameOverState = false;
    bool isGameWinState = false;
    bool hasPlayedEndSound = false;
    SDL_Event event;

     while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
            }

            const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

            if (!isGameOverState && !isGameWinState) {
                handleInput(currentKeyStates);
                updateRabbit();
                obstacleManager.update();
                background.scroll(3);

                redBirdTickCounter += 10;
                if (redBirdTickCounter >= redBirdTickDelay) {
                    redBird.tick();
                    redBirdTickCounter = 0;
                }

                rabbitTickCounter += 10;
                if (rabbitTickCounter >= rabbitTickDelay) {
                    rabbit.tick();
                    rabbitTickCounter = 0;
                }
                if (isGameOver()) {
                    isGameOverState = true;
                }
                if (isGameWin()) {
                    isGameWinState = true;
                }
            }
            if (!hasPlayedEndSound) {
                if (isGameOverState) {
                    audio.playLoseSound();
                    hasPlayedEndSound = true;
                }
                if (isGameWinState) {
                    audio.playWinSound();
                    hasPlayedEndSound = true;
                }
            }
            graphics.prepareScene();
            graphics.render(background);
            graphics.render(110, 50, redBird);
            graphics.render(200, getRabbitY(), rabbit);
            obstacleManager.render(graphics);

            if (isGameOverState) {
                graphics.renderGameOver(notificationBoard);
            }
            if (isGameWinState) {
                graphics.renderGameWin(notificationBoard);
            }
            graphics.presentScene();
            SDL_Delay(16);
    }

    SDL_DestroyTexture(background.texture); background.texture = nullptr;
    SDL_DestroyTexture(redBirdTexture); redBirdTexture = nullptr;
    SDL_DestroyTexture(rabbitTexture); rabbitTexture = nullptr;
    SDL_DestroyTexture(notificationBoard); notificationBoard = nullptr;

    obstacleManager.cleanUp();
    audio.cleanUp();
    graphics.quit();
    return 0;
}
