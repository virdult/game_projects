#include "character.hpp"

Character::Character(float startX, float startY) : x(startX), y(startY) {
    velocityX = 0.0f; velocityY = 0.0f;
    isJumping = false; facingRight = true;
    currentState = CharState::RUNNING;
    updateSprite();
}

void Character::update(float groundLevel, int maxWidth, bool moveLeft, bool moveRight, bool isCrouching) {
    // 1. HORIZONTAL LOGIC
    if (moveRight) { velocityX = 1.0f; facingRight = true; }
    else if (moveLeft) { velocityX = -1.5f; }
    else { velocityX = 0.0f; }

    x += velocityX;

    // 2. SCREEN CONSTRAINTS
    float minLimit = -(float)spriteWidth; 
    float maxLimit = maxWidth * 0.80f;
    isPushingRight = (moveRight && x >= maxLimit);

    if (x < minLimit) x = minLimit;
    if (x > maxLimit) x = maxLimit;

    // 3. PHYSICS & CROUCH LOGIC
    velocityY += 0.2f; 
    y += velocityY;

    if (y >= groundLevel - 5) {
        y = (float)groundLevel - 5;
        velocityY = 0.0f;
        isJumping = false;
    }

    if (isJumping || velocityY < 0) {
        currentState = CharState::JUMPING;
    } else if (isCrouching) {
        currentState = CharState::CROUCHING;
    } else {
        currentState = CharState::RUNNING;
        if (++animTimer > 5) { animFrame = !animFrame; animTimer = 0; }
    }
    
    // 4. MELEE ATTACK UPDATE
    if (isMeleeAttacking) {
        meleeAttackTimer -= 0.030f; // Decrement based on frame time
        if (meleeAttackTimer <= 0) {
            isMeleeAttacking = false;
            meleeCooldownTimer = 0.2f; // Start cooldown
        }
    }
    
    if (meleeCooldownTimer > 0) {
        meleeCooldownTimer -= 0.030f;
    }

    updateSprite();
}

void Character::jump() { if (!isJumping) { velocityY = -2.5f; isJumping = true; } }

void Character::startMeleeAttack() { 
    if (meleeCooldownTimer <= 0 && !isMeleeAttacking) { 
        isMeleeAttacking = true; 
        meleeAttackTimer = 0.3f; // 10 frames * 30ms = 300ms
    } 
}

void Character::updateSprite() {
    //When attacking melee
    if (isMeleeAttacking) {
        sprite = std::vector<std::string>{
            "      ()  ", 
            "     _-  ", 
            "   -/ \\  ", 
            "     /   ", 
            "    /|   ", 
            "   /     "
        };
        return;
    }
    // Normal running/jumping/crouching animations
    if (currentState == CharState::CROUCHING) {
        sprite = std::vector<std::string>{"         ", "         ", "   ()    ", "   ||_~  ", "   ||    ", " _/_\\_   "};
    } else if (currentState == CharState::RUNNING) {
        sprite = animFrame ? std::vector<std::string>{"       ()", "     _-  ", "  --/    ", "     /   ", "    / \\  ", "  _/   > "} 
                           : std::vector<std::string>{"       ()", "     _-  ", "  --/    ", "     /   ", "    < \\  ", "       \\_"};
    } else {
        sprite = std::vector<std::string>{"       ()", "     _-  ", "  --/    ", "     /   ", "    /|   ", "   /     "};
    }
}