#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <string>
#include "character.hpp"
#include "projectiles.hpp"
#include "mobs.hpp"

struct BackgroundObject {
    float x;
    int y;
    int width;
    int height;
    std::vector<std::string> sprite;
    int layer; // 0: Sun, 1: Small Clouds, 2: Mid Background, 3: Foreground
};

class World {
private:
    int width, height;
    int groundLevel;
    float globalOffset = 0.0f;
    std::vector<BackgroundObject> bgObjects;

public:
    std::vector<Projectile> activeProjectiles;
    int maxAmmo = 8;
    int currentAmmo = 8;
    float reloadTimer = 0.0f;
    float cooldownTimer = 0.0f;
    bool isReloading = false;
    
    std::vector<Mob> activeMobs;
    void spawnMob(MobType type, bool fromRight);
    void updateWorld(float deltaTime, float playerX, float playerY, float playerVelX);

    World(int w, int h);
    void addObject(int x, int y, std::vector<std::string> sprite, int layer);
    void generateWorld();
    void updateProjectiles(float deltaTime, float playerVelX);
    void drawFrame(const Character& player);
    void startReload(bool manual);
    int getGroundLevel() const { return groundLevel; }
};

#endif // WORLD_HPP