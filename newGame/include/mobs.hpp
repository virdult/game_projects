#ifndef MOBS_HPP
#define MOBS_HPP

#include <vector>
#include <string>

enum class MobType { SENTRY, CHARGER, ARCHER };

struct Mob {
    float x, y, speed;
    int health, width, height;
    MobType type;
    std::vector<std::string> sprite;
    bool isDead = false, isCharging = false, hasCharged = false;
    float shootTimer = 0.0f, animTimer = 0.0f;

    Mob(float startX, float startY, MobType t);
    void update(float deltaTime, float playerX, float playerY);
    void updateAnimation(float playerX);
};

#endif