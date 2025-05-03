#ifndef _LOGIC_H
#define _LOGIC  _H
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

const int rabbitX = 245;
const int rabbitColliderOffsetY = 45;
const int rabbitColliderW = 130;
const int rabbitColliderH = 80;
const int carrotWidth = 100;
const int carrotHeight = 100;

bool gameOver = false;
bool gameWin = false;
float rabbitY = groundY;
float velocityY = 0.0f;
bool isJumping = false;

int obstaclesCleared = 0;
bool carrotAppeared = false;
float carrotX = SCREEN_WIDTH;
SDL_Texture* carrotTexture = nullptr;

float getRabbitY();
bool isGameOver();
bool isGameWin();
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

    void loadTextures(Graphics& graphics)
        {
        if (!rockTexture) rockTexture = graphics.loadTexture(ROCK_IMG);
        if (!mushroomTexture) mushroomTexture = graphics.loadTexture(MUSHROOM_IMG);
        if (!grassTexture) grassTexture = graphics.loadTexture(GRASS_IMG);
        if (!carrotTexture) carrotTexture = graphics.loadTexture(CARROT_IMG);
    }

    void update() {
        if (isGameOver() || isGameWin()) return;

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastSpawnTime >= OBSTACLE_SPAWN_INTERVAL) {
            spawnObstacle();
            lastSpawnTime = currentTime;
        }

        for (auto& obs : obstacles) {
            obs.x -= OBSTACLE_SPEED;

            if (!obs.passed && obs.x + obs.width < 100) {
                obs.passed = true;
                obstaclesCleared++;

                if (obstaclesCleared >= 30 && !carrotAppeared) {
                    carrotAppeared = true;
                    carrotX = SCREEN_WIDTH;
                }
            }
        }

        obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(),[](const Obstacle& obs) { return obs.x + obs.width < 0; }), obstacles.end());

        SDL_Rect rabbitRect = getRabbitCollider(rabbitY);
        for (const auto& obs : obstacles) {
            if (checkCollisionByType(rabbitRect, obs)) {
                gameOver = true;
                return;
            }
        }
        if (carrotAppeared) {
           SDL_Rect carrotRect = { static_cast<int>(carrotX + 230), static_cast<int>(groundY + 50), carrotWidth, carrotHeight };
            if (checkCollision(rabbitRect, carrotRect)) {
                gameWin = true;
            }
            carrotX -= OBSTACLE_SPEED;
        }
    }

    void render(Graphics& graphics) {
        for (auto& obstacle : obstacles) {
            graphics.render(obstacle.x, obstacle.y, obstacle.texture, obstacle.width, obstacle.height);
        }
         if (carrotAppeared) {
            graphics.render(carrotX + 230, groundY + 50, carrotTexture, carrotWidth, carrotHeight);
        }
    }

    void reset()
    {
        obstacles.clear();
        lastSpawnTime = SDL_GetTicks() + 1000;
        obstaclesCleared = 0;
        carrotAppeared = false;
        gameWin = false;
        carrotX = SCREEN_WIDTH;
    }

    void cleanUp()
    {
        SDL_DestroyTexture(rockTexture); rockTexture = nullptr;
        SDL_DestroyTexture(mushroomTexture); mushroomTexture = nullptr;
        SDL_DestroyTexture(grassTexture); grassTexture = nullptr;
        SDL_DestroyTexture(carrotTexture); carrotTexture = nullptr;
    }

private:
    void spawnObstacle()
    {
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

void initRabbit()
{
    rabbitY = groundY;
    velocityY = 0;
    isJumping = false;
}

void handleInput(const Uint8* keys)
{
    if ((isGameOver() || isGameWin()) || isJumping) return;

    if (keys[SDL_SCANCODE_SPACE]) {
        velocityY = jumpStrength;
        isJumping = true;
    }
}

void updateRabbit()
{
    if (isGameOver() || isGameWin()) return;

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

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b)
{
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
      if (obs.radius > 0) {
        int rabbitCenterX = rabbitRect.x + rabbitRect.w / 2;
        int rabbitCenterY = rabbitRect.y + rabbitRect.h / 2;
        int rabbitRadius = std::min(rabbitRect.w, rabbitRect.h) / 2;

        int obsCenterX = obs.x + obs.radius;
        int obsCenterY = obs.y + obs.radius;

        int dx = rabbitCenterX - obsCenterX;
        int dy = rabbitCenterY - obsCenterY;
        int distSq = dx * dx + dy * dy;
        int radiusSum = rabbitRadius + obs.radius;

        return distSq <= radiusSum * radiusSum;
    }
    return false;
}

SDL_Rect getRabbitCollider(float rabbitY)
{
    return {
        rabbitX,
        static_cast<int>(rabbitY) + rabbitColliderOffsetY,
        rabbitColliderW,
        rabbitColliderH
    };
}

SDL_Rect getObstacleCollider(const Obstacle& obs)
{
    if (obs.radius > 0) {
        return { obs.x + obs.radius, obs.y + obs.radius, obs.radius * 2, obs.radius * 2 };
    }
    return { obs.x + 15, obs.y + 10, obs.width - 30, obs.height - 20 };
}

float getRabbitY() { return rabbitY; }
bool isGameOver() { return gameOver; }
bool isGameWin() { return gameWin; }

void resetGame() {
    initRabbit();
    gameOver = false;
    gameWin = false;
    velocityY = 0;
    isJumping = false;
    obstacleManager.reset();

    SDL_Log("Game reset complete - rabbitY: %.1f, gameOver: %d", rabbitY, gameOver);
}

#endif
