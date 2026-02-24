#include "character.hpp"

Character::Character(float startX, float startY) : x(startX), y(startY) {
    velocityX = 0.0f; velocityY = 0.0f;
    isJumping = false; facingRight = true;
    currentState = CharState::RUNNING;
    updateSprite();
}

void Character::update(float groundLevel, int maxWidth, bool moveLeft, bool moveRight, bool isCrouching) {
    if (moveRight) { velocityX = 1.0f; facingRight = true; }
    else if (moveLeft) { velocityX = -1.5f; }
    else { velocityX = 0.0f; }

    x += velocityX;

    float minLimit = -(float)spriteWidth; 
    float maxLimit = maxWidth * 0.80f;
    isPushingRight = (moveRight && x >= maxLimit);

    if (x < minLimit) x = minLimit;
    if (x > maxLimit) x = maxLimit;

    velocityY += 0.2f; 
    y += velocityY;

    if (y >= groundLevel - (spriteHeight - 1)) {
        y = groundLevel - (spriteHeight - 1);
        velocityY = 0.0f; isJumping = false;
    }

    if (isJumping || velocityY < 0) currentState = CharState::JUMPING;
    else if (isCrouching) currentState = CharState::CROUCHING;
    else {
        currentState = CharState::RUNNING;
        if (++animTimer > 5) { animFrame = !animFrame; animTimer = 0; }
    }
    updateSprite();
}

void Character::jump() { if (!isJumping) { velocityY = -2.5f; isJumping = true; } }

void Character::updateSprite() {
    if (currentState == CharState::CROUCHING) {
        sprite = facingRight ? std::vector<std::string>{"         ", "         ", "   ()    ", "   ||_~  ", "   ||    ", " _/_\\_   "}
                             : std::vector<std::string>{"         ", "         ", "    ()   ", "  ~_||   ", "    ||   ", "  _/_\\_  "};
    } else if (currentState == CharState::RUNNING) {
        if (facingRight) sprite = animFrame ? std::vector<std::string>{"       ()", "     _-  ", "  --/    ", "     /   ", "    / \\  ", "  _/   > "} 
                                            : std::vector<std::string>{"       ()", "     _-  ", "  --/    ", "     /   ", "    < \\  ", "       \\_"};
        else sprite = animFrame ? std::vector<std::string>{"()       ", "   -_    ", "    \\--  ", "   \\     ", "  / \\    ", " <   \\_  "}
                                : std::vector<std::string>{"()       ", "   -_    ", "    \\--  ", "   \\     ", "  / >    ", "_/       "};
    } else {
        sprite = facingRight ? std::vector<std::string>{"       ()", "     _-  ", "  --/    ", "     /   ", "    /|   ", "   /     "}
                             : std::vector<std::string>{"()       ", "   -_    ", "    \\--  ", "   \\     ", "   |\\    ", "    \\    "};
    }
}