#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <vector>
#include <string>

enum class CharState {
    IDLE,
    RUNNING,
    JUMPING,
    CROUCHING
};

class Character {
public:
    float x, y;
    float velocityX, velocityY; // Declared here for world parallax access
    bool isJumping;
    bool facingRight;
    int health = 3;
    float iFrameTimer = 0.0f;

    CharState currentState;
    int animFrame;
    int animTimer;

    std::vector<std::string> sprite;
    int spriteWidth;
    int spriteHeight;

    Character(float startX, float startY);
    
    void update(float groundLevel, int maxWidth, bool moveLeft, bool moveRight, bool isCrouching);
    void jump();
    void updateSprite(); 
};

#endif // CHARACTER_HPP