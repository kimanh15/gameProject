#ifndef AUDIO_H
#define AUDIO_H

#include <SDL_mixer.h>
#include "defs.h"

struct Audio {
    Mix_Music* backgroundMusic = nullptr;
    Mix_Chunk* winSound = nullptr;
    Mix_Chunk* loseSound = nullptr;
    bool audioInitialized = false;


    void loadAudio() {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
            return;
        }
        audioInitialized = true;

        backgroundMusic = Mix_LoadMUS(BGM_PATH);
        if (!backgroundMusic) {
            SDL_Log("Failed to load background music: %s", Mix_GetError());
        }

        winSound = Mix_LoadWAV(WIN_SOUND_PATH);
        if (!winSound) {
            SDL_Log("Failed to load win sound: %s", Mix_GetError());
        }

        loseSound = Mix_LoadWAV(LOSE_SOUND_PATH);
        if (!loseSound) {
            SDL_Log("Failed to load lose sound: %s", Mix_GetError());
        }
    }

    void playBackgroundMusic() {
        if (backgroundMusic) {
            Mix_PlayMusic(backgroundMusic, -1);
        }
    }

    void playWinSound() {
        if (winSound) {
            Mix_PlayChannel(-1, winSound, 0);
        }
    }

    void playLoseSound() {
        if (loseSound) {
            Mix_PlayChannel(-1, loseSound, 0);
        }
    }

    void cleanUp() {
        if (!audioInitialized) return;

        if (backgroundMusic) {
            Mix_FreeMusic(backgroundMusic);
            backgroundMusic = nullptr;
        }
        if (winSound) {
            Mix_FreeChunk(winSound);
            winSound = nullptr;
        }
        if (loseSound) {
            Mix_FreeChunk(loseSound);
            loseSound = nullptr;
        }
        Mix_CloseAudio();
        audioInitialized = false;
    }
    bool isLoaded() const
    {
        return backgroundMusic && winSound && loseSound;
    }
};

#endif
