#ifndef PROJECTILES_HPP
#define PROJECTILES_HPP

class Projectile {
public:
    float x, y;
    float velocityX;
    bool isDead;
    char symbol;

    Projectile(float startX, float startY, float velX);
    void update(int maxWidth);
};

#endif