#include "mobs.hpp"
#include <cmath>

Mob::Mob(float startX, float startY, MobType t) : x(startX), y(startY), type(t) {
    isDead = false; isCharging = false; hasCharged = false;
    shootTimer = 0.0f; animTimer = 0.0f;

    switch (type) {
        case MobType::SENTRY: health = 4; speed = 0; break;
        case MobType::CHARGER: health = 3; speed = 1.2f; break;
        case MobType::ARCHER: health = 2; speed = 0; break;
    }
    updateAnimation(startX + 100); 
    width = (int)sprite[0].length();
    height = (int)sprite.size();
}

void Mob::update(float deltaTime, float playerX, float playerY) {
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
    float dist = std::abs(x - playerX);

    if (type == MobType::ARCHER) {
        // --- ARTIK GÖTÜNDE ÇIKINTI OLMAYAN OKÇU ---
        if (shootTimer >= 1.2f) { // Gerilme
            sprite = { 
                "      ( )  ", 
                "    __|_\\  ", 
                "(-=|  |/   ", 
                "      |    ", 
                "     / \\   ", 
                "    /   \\  " 
            }; 
        } else { // Bekleme
            sprite = { 
                "      ( )  ", 
                "    __|_\\  ", 
                "(--|  |/   ", 
                "      |    ", 
                "     / \\   ", 
                "    /   \\  " 
            };
        }
    } 
    else if (type == MobType::CHARGER) {
        // --- KALKANI YUKARIDA TUTAN CHARGER ---
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
        // --- 10 GERİ 10 İLERİ MIZRAKÇI (SOPASIZ DEĞİL) ---
        if (dist < 25.0f) { 
            animTimer += 0.04f; 
            int frame = (int)(animTimer * 10) % 20;

            if (frame < 10) { // 10 KARE GERİ ÇEKİLME
                sprite = {
                    "      ( )       ",
                    "     /| \\_      ",
                    "   <--|--|-----", 
                    "     / \\        ",
                    "    /   \\       "
                };
            } else { // 10 KARE İLERİ SAPLAMA
                sprite = {
                    "      ( )       ",
                    "     /| \\_      ",
                    "<---/------|--  ", 
                    "     / \\        ",
                    "    /   \\       "
                };
            }
        } else { // IDLE MIZRAKÇI
            animTimer = 0;
            sprite = {
                "      ( )       ",
                "     /| \\_      ",
                "   <---|--|---  ", 
                "      / \\       ",
                "     /   \\      "
            };
        }
    }
    width = (int)sprite[0].length();
    height = (int)sprite.size();
}