#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <vector>
#include <string>

enum class CharState { RUNNING, JUMPING, CROUCHING };

class Character {
public:
    float x, y, velocityX, velocityY;
    bool isJumping, facingRight, isPushingRight = false;
    int health = 3;
    float iFrameTimer = 0.0f;

    CharState currentState;
    int animFrame = 0, animTimer = 0;
    std::vector<std::string> sprite;
    int spriteWidth = 9, spriteHeight = 6;

    Character(float startX, float startY);
    void update(float groundLevel, int maxWidth, bool moveLeft, bool moveRight, bool isCrouching);
    void jump();
    void updateSprite(); 
};

#endif