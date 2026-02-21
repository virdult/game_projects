#include "projectiles.hpp"

Projectile::Projectile(float startX, float startY, float velX) {
    x = startX;
    y = startY;
    velocityX = velX;
    symbol = '.';
    isDead = false;
}

void Projectile::update(int maxWidth) {
    x += velocityX;
    // Mark as dead if it flies off the screen
    if (x < 0 || x >= maxWidth) {
        isDead = true;
    }
}