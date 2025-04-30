#ifndef _LOGIC__H
#define _LOGIC__H
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include "defs.h"
#include "graphics.h"


const float gravity = 0.30f;
const float jumpStrength = -10.0;
const float groundY = 380.0f;
const float gravityUp = 0.20f;
const float gravityDown = 0.025f;
const float maxJumpHeight = 240.0f;

bool gameOver = false;
float rabbitY = groundY;
float velocityY = 0.0f;
bool isJumping = false;

float getRabbitY();
bool isGameOver();
void resetGame();
void initRabbit();
void handleInput(const Uint8* keys);
void updateRabbit();
bool checkCollision(const SDL_Rect& a, const SDL_Rect& b);
bool checkCollisionByType(const SDL_Rect& rabbitRect, const Obstacle& obs);
SDL_Rect getRabbitCollider(float rabbitY);
SDL_Rect getObstacleCollider(const Obstacle& obs);

class ObstacleManager {
private:
    std::vector<Obstacle> obstacles;
    SDL_Texture* rockTexture = nullptr;
    SDL_Texture* mushroomTexture = nullptr;
    SDL_Texture* grassTexture = nullptr;
    Uint32 lastSpawnTime = 0;

public:
    const std::vector<Obstacle>& getObstacles() const {
        return obstacles;
    }

    void loadTextures(Graphics& graphics) {
        rockTexture = graphics.loadTexture(ROCK_IMG);
        mushroomTexture = graphics.loadTexture(MUSHROOM_IMG);
        grassTexture = graphics.loadTexture(GRASS_IMG);
    }

    void update() {
         if (isGameOver()) return;

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastSpawnTime >= OBSTACLE_SPAWN_INTERVAL) {
            spawnObstacle();
            lastSpawnTime = currentTime;
        }

        for (auto& obstacle : obstacles) {
            obstacle.x -= OBSTACLE_SPEED;
        }

        obstacles.erase(
            std::remove_if(obstacles.begin(), obstacles.end(),
                [](const Obstacle& obs) { return obs.x + obs.width < 0; }),
            obstacles.end()
        );
        SDL_Rect rabbitRect = getRabbitCollider(rabbitY);
        for (const auto& obs : obstacles) {
            SDL_Rect obsRect = getObstacleCollider(obs);

            if (checkCollisionByType(rabbitRect, obs)) {
                gameOver = true;
                break;
            }
        }
    }
    void render(Graphics& graphics) {
        for (auto& obstacle : obstacles) {
            graphics.render(obstacle.x, obstacle.y, obstacle.texture, obstacle.width, obstacle.height);
        }
    }

    void reset() {
        obstacles.clear();
        lastSpawnTime = SDL_GetTicks() + 1000;
    }

    void cleanUp() {
        SDL_DestroyTexture(rockTexture); rockTexture = nullptr;
        SDL_DestroyTexture(mushroomTexture); mushroomTexture = nullptr;
        SDL_DestroyTexture(grassTexture); grassTexture = nullptr;
    }

private:
    void spawnObstacle() {
        int type = rand() % 3;
        Obstacle newObstacle;
        newObstacle.x = SCREEN_WIDTH;
        newObstacle.y = groundY + 50;

        switch (type) {
        case 0:
            newObstacle.texture = rockTexture;
            newObstacle.width = 140;
            newObstacle.height = 140;
            newObstacle.radius = 70;
            newObstacle.type = "rock";
            break;
        case 1:
            newObstacle.texture = mushroomTexture;
            newObstacle.width = 120;
            newObstacle.height = 120;
            newObstacle.radius = 0;
            newObstacle.type = "mushroom";
            break;
        case 2:
            newObstacle.texture = grassTexture;
            newObstacle.width = 140;
            newObstacle.height = 140;
            newObstacle.radius = 70;
            newObstacle.type = "grass";
            break;
    }

    obstacles.push_back(newObstacle);
}
};
ObstacleManager obstacleManager;

void initRabbit() {
    rabbitY = groundY;
    velocityY = 0;
    isJumping = false;
}

void handleInput(const Uint8* keys) {
     if (isGameOver()) return;

    if (keys[SDL_SCANCODE_SPACE] && !isJumping) {
        velocityY = jumpStrength;
        isJumping = true;
    }
}

void updateRabbit() {
    if (isGameOver()) return;

    velocityY += (velocityY < 0) ? gravityUp : gravityDown;
    rabbitY += velocityY;

    if (rabbitY < maxJumpHeight) {
        rabbitY = maxJumpHeight;
        velocityY = 0;
    }

    if (rabbitY >= groundY) {
        rabbitY = groundY;
        velocityY = 0;
        isJumping = false;
    }
}

float getRabbitY() {
    return rabbitY;
}

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

bool checkCollisionByType(const SDL_Rect& rabbitRect, const Obstacle& obs) {
   if (obs.type == "mushroom") {
    SDL_Rect obsRect = getObstacleCollider(obs);
    return checkCollision(rabbitRect, obsRect);
    }
     else if (obs.type == "rock" || obs.type == "grass") {
     if (obs.radius > 0) {
            int rabbitCenterX = rabbitRect.x + rabbitRect.w / 2;
            int rabbitCenterY = rabbitRect.y + rabbitRect.h / 2;
            int rabbitRadius = std::min(rabbitRect.w, rabbitRect.h) / 2;

            int obsCenterX = obs.x + obs.radius;
            int obsCenterY = obs.y + obs.radius;

            int obsRadius;
            if (obs.type == "rock") {
                obsRadius = 65;
            } else if (obs.type == "grass") {
                obsRadius = 70;
            } else {
                obsRadius = obs.radius;
            }
            int dx = rabbitCenterX - obsCenterX;
            int dy = rabbitCenterY - obsCenterY;
            int distSq = dx * dx + dy * dy;
            int radiusSum = rabbitRadius + obsRadius;

            return distSq <= radiusSum * radiusSum;
        }
    }

    return false;
}

SDL_Rect getRabbitCollider(float rabbitY) {
     return {
        245,
        static_cast<int>(rabbitY) + 45,
        130,
        80
    };
}

SDL_Rect getObstacleCollider(const Obstacle& obs) {
        if (obs.radius > 0) {
        return { obs.x + obs.radius, obs.y + obs.radius, obs.radius * 2, obs.radius * 2 };
    }
    return { obs.x + 15, obs.y + 10, obs.width - 30, obs.height - 20 };
}


bool isGameOver() {
    return gameOver;
}

void resetGame() {
    initRabbit();
    gameOver = false;
    velocityY = 0;
    isJumping = false;
    obstacleManager.reset();

    SDL_Log("Game reset complete - rabbitY: %.1f, gameOver: %d", rabbitY, gameOver);
}

#endif
