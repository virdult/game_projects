#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <string>
#include "character.hpp"
#include "projectiles.hpp"
#include "mobs.hpp"

struct Obstacle {
    float x, y;
    int width, height;
    std::vector<std::string> sprite;
    bool isSolid = true;
    bool isHazard = false;
};

class World {
private:
    int width, height, groundLevel;
    float globalOffset = 0.0f, nextSpawnX = 0.0f;
    int lastSegmentType = -1; 
    std::vector<Obstacle> obstacles;
    void resetStateInitialization(); 

public:
    std::vector<Projectile> activeProjectiles, enemyProjectiles;
    std::vector<Mob> activeMobs;
    int score = 0;
    float cooldownTimer = 0.0f, autoScrollSpeed = 0.5f;
    bool gameOver = false;

    World(int w, int h);
    void updateWorld(float deltaTime, Character& player);
    void drawFrame(const Character& player);
    void spawnSegment(int type, float startX);
    void reset(Character& player);
    int getGroundLevel() const { return groundLevel; }
};

#endif