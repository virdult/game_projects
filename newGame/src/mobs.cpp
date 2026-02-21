#include "mobs.hpp"
#include <cmath>
#include <cstdlib>

Mob::Mob(float startX, float startY, MobType t) 
    : x(startX), y(startY), type(t), originalY(startY) {
    
    switch (type) {
        case MobType::CRAWLER: 
            health = 6;     // Tanky Crawler
            speed = 0.35f;  // Even slower
            sprite = {
                "  ___/\\____  ", 
                " (_________) "
            };
            break;
        case MobType::WALKER: 
            health = 3; 
            speed = 0.15f;  // Very slow heavy walker
            sprite = {
                "    __________    ", 
                "   [__________]   ",
                "   /|  [o]  |\\    ",
                "    |       |     ",
                "    /       \\     "
            };
            break;
        case MobType::FLYER: 
            health = 1;
            speed = 0.4f;   // Slower flight
            sprite = {
                " --==[O]==-- ",
                "    /   \\    "
            };
            break;
    }
    width = sprite[0].length();
    height = sprite.size();
}

void Mob::update(float deltaTime, float playerX, float playerY) {
    if (type == MobType::FLYER) {
        if (!isRetreating) {
            // 1. Move toward player horizontally
            if (x < playerX) x += speed;
            else x -= speed;

            // 2. Descend when close
            float distToPlayerX = std::abs(x - playerX);
            if (distToPlayerX < 25.0f) {
                if (y < playerY) y += 0.2f; // Dive
            }

            // 3. Trigger retreat if very close
            if (distToPlayerX < 3.0f && std::abs(y - playerY) < 5.0f) {
                isRetreating = true;
                retreatTargetX = x + (rand() % 2 == 0 ? 40.0f : -40.0f);
            }
        } else {
            // 4. Retreat to random X and original Y
            if (x < retreatTargetX) x += speed * 2; // Move away faster
            else x -= speed * 2;

            if (y > originalY) y -= 0.2f; // Climb back up

            // 5. Reset once reached retreat target
            if (std::abs(x - retreatTargetX) < 2.0f && std::abs(y - originalY) < 2.0f) {
                isRetreating = false;
            }
        }
    } else {
        // Normal Ground Movement
        if (x < playerX) x += speed;
        else x -= speed;
    }
}