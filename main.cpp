
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

    Mouse mouse;
    mouse.x = SCREEN_WIDTH / 2;
    mouse.y = SCREEN_HEIGHT / 2;

    ScrollingBackground background;
    background.setTexture(graphics.loadTexture(BACKGROUND_IMG));

    Sprite redBird;
    SDL_Texture* redBirdTexture = graphics.loadTexture(RED_BIRD_SPRITE_FILE);
    redBird.init(redBirdTexture, RED_BIRD_FRAMES, RED_BIRD_CLIPS);

    Sprite greenBird;
    SDL_Texture* greenBirdTexture = graphics.loadTexture(GREEN_BIRD_SPRITE_FILE);
    greenBird.init(greenBirdTexture, GREEN_BIRD_FRAMES, GREEN_BIRD_CLIPS);

    Sprite rabbit;
    SDL_Texture* rabbitTexture = graphics.loadTexture(RABBIT_SPRITE_FILE);
    rabbit.init(rabbitTexture, RABBIT_FRAMES, RABBIT_CLIPS);

    int redBirdTickCounter = 0;
    int greenBirdTickCounter = 0;
    int rabbitTickCounter = 0;

    bool quit = false;
    SDL_Event event;
     while (!quit) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
        }
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_UP]) mouse.turnNorth();
        if (currentKeyStates[SDL_SCANCODE_DOWN]) mouse.turnSouth();
        if (currentKeyStates[SDL_SCANCODE_LEFT]) mouse.turnWest();
        if (currentKeyStates[SDL_SCANCODE_RIGHT]) mouse.turnEast();

        mouse.move();
        background.scroll(1);

        redBirdTickCounter += 10;
        if (redBirdTickCounter >= redBirdTickDelay) {
            redBird.tick();
            redBirdTickCounter = 0;
        }

        greenBirdTickCounter += 10;
        if (greenBirdTickCounter >= greenBirdTickDelay) {
            greenBird.tick();
            greenBirdTickCounter = 0;
        }

        rabbitTickCounter += 10;
        if (rabbitTickCounter >= rabbitTickDelay) {
            rabbit.tick();
            rabbitTickCounter = 0;
        }

        graphics.prepareScene();
        graphics.render(background);
        render(mouse, graphics);

        graphics.render(150, 100, redBird);
        graphics.render(550, 150, greenBird);
        graphics.render(200, 370, rabbit);


        graphics.presentScene();

        SDL_Delay(10);
    }

    SDL_DestroyTexture(background.texture); background.texture = nullptr;
    SDL_DestroyTexture(redBirdTexture); redBirdTexture = nullptr;
    SDL_DestroyTexture(greenBirdTexture); greenBirdTexture = nullptr;
    SDL_DestroyTexture(rabbitTexture); rabbitTexture = nullptr;

    graphics.quit();
    return 0;
}

