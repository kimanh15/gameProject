#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include "defs.h"

struct ScrollingBackground {
    SDL_Texture* texture;
    int scrollingOffset = 0;
    int width, height;

    void setTexture(SDL_Texture* _texture)
    {
        texture = _texture;
        SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    }

    void scroll(int distance)
    {
        scrollingOffset -= distance;
        if (scrollingOffset <= -width) {
        scrollingOffset += width;
        }
    }
    void setX(int newX)
    {
        scrollingOffset = newX;
    }
};

struct Sprite {
    SDL_Texture* texture;
    std::vector<SDL_Rect> clips;
    int currentFrame = 0;

    void init(SDL_Texture* _texture, int frames, const int _clips [][4])
    {
        texture = _texture;

        SDL_Rect clip;
        for (int i = 0; i < frames; i++) {
            clip.x = _clips[i][0];
            clip.y = _clips[i][1];
            clip.w = _clips[i][2];
            clip.h = _clips[i][3];
            clips.push_back(clip);
        }
    }
    void tick()
    {
        currentFrame = (currentFrame + 1) % clips.size();
    }

    const SDL_Rect* getCurrentClip() const
    {
        return &(clips[currentFrame]);
    }
     void reset()
    {
        currentFrame = 0;
    }
};

struct Graphics {
    SDL_Renderer *renderer;
	SDL_Window *window;
    TTF_Font* font = nullptr;

	void logErrorAndExit(const char* msg, const char* error)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
        SDL_Quit();
        exit(1);
    }

	void init()
	{
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
            logErrorAndExit("SDL_Init", SDL_GetError());

        window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (window == nullptr) logErrorAndExit("CreateWindow", SDL_GetError());

        if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
            logErrorAndExit( "SDL_image error:", IMG_GetError());

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                              SDL_RENDERER_PRESENTVSYNC);

        if (renderer == nullptr) logErrorAndExit("CreateRenderer", SDL_GetError());

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

        if (TTF_Init() == -1) {
        logErrorAndExit("TTF_Init", TTF_GetError());
        }
        font = TTF_OpenFont("arial.ttf", 22);
        if (!font) {
            logErrorAndExit("Failed to load font", TTF_GetError());
        }
    }

    void prepareScene()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

	void prepareScene(SDL_Texture * background)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
    }

    void render(const ScrollingBackground& bgr)
    {
        renderTexture(bgr.texture, bgr.scrollingOffset, 0);
        renderTexture(bgr.texture, bgr.scrollingOffset + bgr.width, 0);
    }

    void renderTexture(SDL_Texture *texture, int x, int y)
    {
        int texW, texH;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
        dest.w = texW;
        dest.h = SCREEN_HEIGHT;

        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }


    void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y)
    {
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;

        if (src) {
            dest.w = src->w;
            dest.h = src->h;
        } else {
            SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
        }

        SDL_RenderCopy(renderer, texture, src, &dest);
    }

    void presentScene()
    {
        SDL_RenderPresent(renderer);
    }

    SDL_Texture *loadTexture(const char *filename)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

        SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
         if (texture == NULL) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture failed: %s", IMG_GetError());
            return nullptr;
        }
        return texture;
    }

    void quit()
    {
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }

        TTF_Quit();
        IMG_Quit();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void render(int x, int y, const Sprite& sprite)
    {
        const SDL_Rect* clip = sprite.getCurrentClip();
        SDL_Rect renderQuad = {x, y, clip->w, clip->h};
        SDL_RenderCopy(renderer, sprite.texture, clip, &renderQuad);
    }
    void render(int x, int y, SDL_Texture* texture, int width, int height)
    {
        SDL_Rect dest = {x, y, width, height};
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }
    void renderObstacle(float x, float y, float radius, SDL_Texture* texture)
    {
        SDL_Rect dest;
        dest.x = static_cast<int>(x - radius);
        dest.y = static_cast<int>(y - radius);
        dest.w = static_cast<int>(radius * 2);
        dest.h = static_cast<int>(radius * 2);
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    void renderGameOver(SDL_Texture* notificationBoard)
    {
        int boardWidth = 400;
        int boardHeight = 200;
        int boardX = (SCREEN_WIDTH - boardWidth) / 2;
        int boardY = (SCREEN_HEIGHT - boardHeight) / 2;
        SDL_Rect dest = { boardX, boardY, boardWidth, boardHeight };
        SDL_RenderCopy(renderer, notificationBoard, NULL, &dest);
        int textMaxWidth = boardWidth - 40;
        int textX = boardX + boardWidth / 2;
        int textY = boardY + boardHeight / 2;
        renderText("You Lost!", textX, textY, textMaxWidth);
    }
    void renderGameWin(SDL_Texture* notificationBoard)
    {
        int boardWidth = 400;
        int boardHeight = 200;
        int boardX = (SCREEN_WIDTH - boardWidth) / 2;
        int boardY = (SCREEN_HEIGHT - boardHeight) / 2;
        SDL_Rect dest = { boardX, boardY, boardWidth, boardHeight };
        SDL_RenderCopy(renderer, notificationBoard, NULL, &dest);
        int textMaxWidth = boardWidth - 40;
        int textX = boardX + boardWidth / 2;
        int textY = boardY + boardHeight / 2;
        renderText("Congratulations! You win!", textX, textY, textMaxWidth);
    }

    void renderText(const std::string& message, int x, int y, int maxWidth)
    {
        SDL_Color textColor = { 165, 104, 73, 255 };
        SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, message.c_str(), textColor, maxWidth);
        if (textSurface != nullptr) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect renderQuad = {
            x - textSurface->w / 2,
            y - textSurface->h / 2,
            textSurface->w,
            textSurface->h
        };
        SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        } else {
            SDL_Log("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        }
    }

};


#endif
