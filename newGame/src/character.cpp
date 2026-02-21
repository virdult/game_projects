#include "character.hpp"

Character::Character(float startX, float startY) {
    x = startX;
    velocityX = 0.0f; // Initialized
    velocityY = 0.0f;
    isJumping = false;
    facingRight = true;
    
    currentState = CharState::IDLE;
    animFrame = 0;
    animTimer = 0;

    spriteHeight = 6; 
    spriteWidth = 9;  

    y = startY - (spriteHeight - 1.0f); 
    
    updateSprite();
}

void Character::update(float groundLevel, int maxWidth, bool moveLeft, bool moveRight, bool isCrouching) {
    // 1. INPUT HANDLING & VELOCITY
    if (moveLeft) {
        velocityX = -2.0f;
        facingRight = false;
    } else if (moveRight) {
        velocityX = 2.0f;
        facingRight = true;
    } else {
        velocityX = 0.0f;
    }

    // 2. GRAVITY & PHYSICS
    velocityY += 0.2f; 
    x += velocityX;
    y += velocityY;

    // 3. GROUND COLLISION
    if (y >= groundLevel - (spriteHeight - 1)) {
        y = groundLevel - (spriteHeight - 1);
        velocityY = 0.0f;
        isJumping = false;
    }

    // 4. BOUNDARY CHECK
    if (x < 0) x = 0;
    if (x > maxWidth - spriteWidth) x = maxWidth - spriteWidth;

    // 5. STATE MACHINE
    if (isJumping || velocityY != 0.0f) {
        currentState = CharState::JUMPING;
    } 
    else if (isCrouching) {
        currentState = CharState::CROUCHING;
    } 
    else if (moveLeft || moveRight || velocityX != 0.0f) {
        currentState = CharState::RUNNING;
        animTimer++;
        if (animTimer > 5) {
            animFrame = (animFrame == 0) ? 1 : 0;
            animTimer = 0;
        }
    } 
    else {
        currentState = CharState::IDLE;
    }

    updateSprite();
}

void Character::jump() {
    if (!isJumping) {
        velocityY = -2.5f; 
        isJumping = true;
    }
}

void Character::updateSprite() {
    if (currentState == CharState::IDLE) {
        if (facingRight) {
            sprite = { "  ()     ", "  ||_~   ", "  ||     ", "  ||     ", "  ||     ", " _||     " };
        } else {
            sprite = { "     ()  ", "   ~_||  ", "     ||  ", "     ||  ", "     ||  ", "     ||_ " };
        }
    } 
    else if (currentState == CharState::CROUCHING) {
        if (facingRight) {
            sprite = { "         ", "         ", "   ()    ", "   ||_~  ", "   ||    ", " _/_\\_   " };
        } else {
            sprite = { "         ", "         ", "    ()   ", "  ~_||   ", "    ||   ", "  _/_\\_  " };
        }
    }
    else if (currentState == CharState::RUNNING) {
        if (facingRight) {
            if (animFrame == 0) {
                sprite = { "       ()", "     _-  ", "  --/    ", "     /   ", "    / \\  ", "  _/   > " };
            } else {
                sprite = { "       ()", "     _-  ", "  --/    ", "     /   ", "    < \\  ", "       \\_" };
            }
        } else {
            if (animFrame == 0) {
                sprite = { "()       ", "   -_    ", "    \\--  ", "   \\     ", "  / \\    ", " <   \\_  " };
            } else {
                sprite = { "()       ", "   -_    ", "    \\--  ", "   \\     ", "  / >    ", "_/       " };
            }
        }
    } 
    else if (currentState == CharState::JUMPING) {
        if (facingRight) {
            sprite = { "       ()", "     _-  ", "  --/    ", "     /   ", "    /|   ", "   /     " };
        } else {
            sprite = { "()       ", "   -_    ", "    \\--  ", "   \\     ", "   |\\    ", "    \\    " };
        }
    }
}