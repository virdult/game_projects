#ifndef MOBS_HPP
#define MOBS_HPP

#include <vector>
#include <string>

enum class MobType { CRAWLER, WALKER, FLYER };

struct Mob {
    float x, y;
    float speed;
    int health;
    int width, height;
    MobType type;
    std::vector<std::string> sprite;
    bool isDead = false;
    
    // Flyer specific logic
    float originalY;
    bool isRetreating = false;
    float retreatTargetX = 0;

    Mob(float startX, float startY, MobType t);
    void update(float deltaTime, float playerX, float playerY);
};

#endif