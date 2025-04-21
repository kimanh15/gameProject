#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include "defs.h"
#include "graphics.h"
#include "logic.h"

using namespace std;

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

    ScrollingBackground background;
    background.setTexture(graphics.loadTexture(BACKGROUND_IMG));

    Sprite redBird;
    SDL_Texture* redBirdTexture = graphics.loadTexture(RED_BIRD_SPRITE_FILE);
    redBird.init(redBirdTexture, RED_BIRD_FRAMES, RED_BIRD_CLIPS);

    Sprite rabbit;
    SDL_Texture* rabbitTexture = graphics.loadTexture(RABBIT_SPRITE_FILE);
    rabbit.init(rabbitTexture, RABBIT_FRAMES, RABBIT_CLIPS);

    int redBirdTickCounter = 0;
    int rabbitTickCounter = 0;

    initRabbit();

    bool quit = false;
    SDL_Event event;
     while (!quit) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
        }
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        handleInput(currentKeyStates);
        updateRabbit();

        background.scroll(2);

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

        graphics.prepareScene();
        graphics.render(background);
        graphics.render(110, 50, redBird);
        graphics.render(200, getRabbitY(), rabbit);

        graphics.presentScene();

        SDL_Delay(10);
    }

    SDL_DestroyTexture(background.texture); background.texture = nullptr;
    SDL_DestroyTexture(redBirdTexture); redBirdTexture = nullptr;
    SDL_DestroyTexture(rabbitTexture); rabbitTexture = nullptr;

    graphics.quit();
    return 0;
}

