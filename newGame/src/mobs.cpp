#include "mobs.hpp"
#include <cmath>

Mob::Mob(float startX, float startY, MobType t) : x(startX), y(startY), type(t) {
    isDead = false; isDying = false; isCharging = false; hasCharged = false;
    shootTimer = 0.0f; animTimer = 0.0f; deathTimer = 0.0f;

    switch (type) {
        case MobType::SENTRY: health = 3; speed = 0; break;
        case MobType::CHARGER: health = 4; speed = 1.2f; break;
        case MobType::ARCHER: health = 2; speed = 0; break;
    }
    updateAnimation(startX + 100); 
    width = (int)sprite[0].length();
    height = (int)sprite.size();
}

void Mob::update(float deltaTime, float playerX, float playerY) {
    // 1. If the mob is dying, stop all AI and just play the death animation
    if (isDying) {
        deathTimer += deltaTime;
        updateAnimation(playerX);
        
        // After 0.9 seconds of dying, finally mark as completely dead for deletion
        if (deathTimer >= 0.9f) {
            isDead = true; 
        }
        return; // Skip the rest of the movement/attack logic
    }

    // 2. Normal AI Logic
    float dist = x - playerX;
    if (type == MobType::CHARGER) {
        if (!hasCharged && !isCharging && dist > 0 && dist < 50.0f) isCharging = true;
        if (isCharging) {
            x -= (speed * 4.0f);
            if (x < playerX - 15.0f) { isCharging = false; hasCharged = true; }
        }
    }
    if (type == MobType::ARCHER) shootTimer += deltaTime;
    updateAnimation(playerX);
}

void Mob::updateAnimation(float playerX) {
    // --- UNIVERSAL DEATH ANIMATION ---
    if (isDying) {
        if (deathTimer < 0.15f) { 
            sprite = {
                "           ",
                "   (>_<)   ",
                "  --/|\\--  ",
                "     |     ",
                "    / \\    ",
                "  _/   \\_  "
            };
        } else if (deathTimer < 0.30f) { 
            sprite = {
                "           ",
                "           ",
                "   (x_x)  ",
                "    /|\\  ",
                "   / | \\ ",
                "  _ _LL___"
            };
        } else if (deathTimer < 0.60f) { 
            sprite = {
                "           ",
                "           ",
                "(x_x)      ",
                "  \\\\     ",
                "   \\\\   ",
                " ___L\\_   "
            };
        } else if (deathTimer < 0.72f) { 
            sprite = {
                "           ",
                "           ",
                "           ",
                "           ",
                "           ",
                " Q=====C   "       
            };
            
            
            
        } else if (deathTimer < 0.84f) { 
            sprite = {
                "           ",
                "           ",
                "           ",
                "           ",
                "   .   .   ",
                "  .  .   . "
            };
        } else { 
            sprite = {
                "           ",
                "           ",
                " .   .   . ",
                "  .  .  .  ",
                "   .   .   "
                "           "
            };
        }
        return; 
    }

    // --- ALIVE ANIMATIONS ---
    float dist = std::abs(x - playerX);

    if (type == MobType::ARCHER) {
        if (shootTimer >= 1.2f) {
            sprite = { "      ( )  ", "    __|_\\  ", "(-=|  |/   ", "      |    ", "     / \\   ", "    /   \\  " }; 
        } else { 
            sprite = { "      ( )  ", "    __|_\\  ", "(--|  |/   ", "      |    ", "     / \\   ", "    /   \\  " };
        }
    } 
    else if (type == MobType::CHARGER) {
        if (isCharging) { 
            sprite = { "     ( )_  ", "    /|_|[| ", "   / | |[| ", "  /  |_|[| ", " _\\_  /    ", " \\__\\/     " }; 
        } else if (dist < 70.0f && !hasCharged) { 
            animTimer += 0.03f;
            if ((int)(animTimer * 5) % 2 == 0)
                sprite = { "   ( )     ", " [|]|_|    ", " [|]| |    ", " [|]| |    ", "  _||      ", "   ||      " };
            else
                sprite = { "   ( )     ", " [|]|_|    ", " [|]| |    ", " [|]| |    ", "   ||_     ", "   ||      " };
        } else { 
            sprite = { "   ( )     ", " [|]|_|    ", " [|]| |    ", " [|]| |    ", "   / \\     ", "  /   \\    " };
        }
    }
    else if (type == MobType::SENTRY) {
        if (dist < 25.0f) { 
            animTimer += 0.04f; 
            int frame = (int)(animTimer * 10) % 20;

            if (frame < 10) {
                sprite = { "      ( )       ", "     /| \\_      ", "   <--|--|-----", "     / \\        ", "    /   \\       " };
            } else {
                sprite = { "      ( )       ", "     /| \\_      ", "<---/------|--  ", "     / \\        ", "    /   \\       " };
            }
        } else {
            animTimer = 0;
            sprite = { "      ( )       ", "     /| \\_      ", "   <---|--|---  ", "      / \\       ", "     /   \\      " };
        }
    }
    width = (int)sprite[0].length();
    height = (int)sprite.size();
}