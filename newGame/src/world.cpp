#include "world.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

World::World(int w, int h) : width(w), height(h) {
    groundLevel = height - 6;
    resetStateInitialization(); 
}

void World::resetStateInitialization() {
    score = 0; gameOver = false; globalOffset = 0; nextSpawnX = 0;
    currentAmmo = maxAmmo; isReloading = false; lastSegmentType = -1;
    obstacles.clear(); activeMobs.clear(); activeProjectiles.clear(); enemyProjectiles.clear();
    
    int first = rand() % 3;
    spawnSegment(first, 100);
    lastSegmentType = first;
    
    int second;
    do { second = rand() % 3; } while (second == lastSegmentType);
    spawnSegment(second, 250);
    lastSegmentType = second;
}

void World::spawnSegment(int type, float startX) {
    float segWidth = 150.0f;
    float spawnY = (float)groundLevel - 6;

    switch(type) {
        case 0: // GAUNTLET
            for(int i=0; i<3; i++) {
                Obstacle h; h.x = startX + (i*50) + 20; h.y = groundLevel - 2;
                h.width = 4; h.height = 2; h.isSolid = true; h.sprite = {"|--|", "|__|"};
                obstacles.push_back(h);
                activeMobs.emplace_back(h.x + 15, spawnY, MobType::CHARGER);
            }
            break;
        case 1: // ROOFTOPS
            for(int i=0; i<2; i++) {
                Obstacle b; b.x = startX + (i*70) + 10; b.y = groundLevel - 12;
                b.width = 30; b.height = 12; b.isSolid = true; b.sprite = std::vector<std::string>(12, std::string(30, '='));
                obstacles.push_back(b);
                activeMobs.emplace_back(b.x + 10, b.y - 6, MobType::ARCHER);
            }
            break;
        case 2: // SENTRY WALL
            for(int i=0; i<3; i++) {
                Obstacle w; w.x = startX + (i*45) + 15; w.y = groundLevel - 5;
                w.width = 2; w.height = 5; w.isSolid = true; w.sprite = {"##","##","##","##","##"};
                obstacles.push_back(w);
                activeMobs.emplace_back(w.x + 8, spawnY, MobType::SENTRY);
            }
            break;
    }
    nextSpawnX = startX + segWidth;
}

void World::updateWorld(float deltaTime, Character& player) {
    if (gameOver) return;

    // 1. Map Generation
    if (nextSpawnX - globalOffset < width + 100) {
        int newType;
        do { newType = rand() % 3; } while (newType == lastSegmentType);
        lastSegmentType = newType;
        spawnSegment(newType, nextSpawnX);
    }
    
    float currentSpeed = player.isPushingRight ? (autoScrollSpeed * 3.0f) : autoScrollSpeed;
    globalOffset += currentSpeed;

    // Death Check
    if (player.x + player.spriteWidth <= 0) { gameOver = true; return; }
    if (isReloading) { reloadTimer -= deltaTime; if (reloadTimer <= 0) { currentAmmo = maxAmmo; isReloading = false; } }
    if (player.iFrameTimer > 0) player.iFrameTimer -= deltaTime;

    // 2. Obstacle & Player Collision (Solid Walls)
    for (auto& obs : obstacles) {
        obs.x -= currentSpeed;
        
        // If the obstacle is solid and at player's height
        if (obs.isSolid && player.y + player.spriteHeight > obs.y + 1) {
            // Horizontal Block: Player hits the left side of the wall
            if (player.x + player.spriteWidth > obs.x && player.x < obs.x) {
                player.x = obs.x - player.spriteWidth; 
            }
        }
    }

    // 3. Projectile Updates with TUNNELING FIX
    for (auto& p : activeProjectiles) {
        float oldX = p.x; // Record position BEFORE update
        p.x -= currentSpeed; 
        p.update(width);  // p.x is now the NEW position
        float newX = p.x;

        for (const auto& obs : obstacles) {
            if (obs.isSolid) {
                // Check if the bullet's Y is within the building height
                if (p.y >= obs.y && p.y <= obs.y + obs.height) {
                    // INTERVAL CHECK: Did the bullet cross the 'obs.x' threshold this frame?
                    // We check if the wall's X boundary lies between oldX and newX
                    if (oldX < obs.x && newX >= obs.x) {
                        p.isDead = true; 
                        p.x = obs.x; // Snap to wall for visual impact
                    }
                }
            }
        }
    }

    for (auto& ep : enemyProjectiles) { ep.x -= currentSpeed; ep.update(width); }

    // 4. Mob Logic
    for (auto& m : activeMobs) {
        m.x -= currentSpeed; m.update(deltaTime, player.x, player.y);

        // Spearman (Sentry) Collision Logic (Using your 10/10 frames)
        if (m.type == MobType::SENTRY) {
            int frame = (int)(m.animTimer * 10) % 20;
            if (frame >= 10) { // Thrusting phase
                if (player.x < m.x && player.x > m.x - 14.0f && std::abs(player.y - m.y) < 4 && player.iFrameTimer <= 0) {
                    player.health--; player.iFrameTimer = 1.5f;
                    if (player.health <= 0) gameOver = true;
                }
            }
        }

        if (m.type == MobType::ARCHER && m.shootTimer > 2.2f) { 
            enemyProjectiles.emplace_back(m.x, m.y + 1, -2.5f); 
            m.shootTimer = 0; 
        }

        // Standard Body Collision
        if (std::abs(player.x - m.x) < 4 && std::abs(player.y - m.y) < 3 && player.iFrameTimer <= 0) {
            player.health--; player.iFrameTimer = 1.5f; if (player.health <= 0) gameOver = true;
        }

        // Bullet vs Mob
        for (auto& p : activeProjectiles) {
            if (!p.isDead && p.x >= m.x && p.x <= m.x + m.width && p.y >= m.y && p.y <= m.y + m.height) {
                m.health--; p.isDead = true;
                if (m.health <= 0) { m.isDead = true; score++; }
            }
        }
    }

    // 5. Cleanup & Platform Landing
    for (auto& ep : enemyProjectiles) {
        if (ep.x >= player.x + 2 && ep.x <= player.x + 7 && ep.y >= player.y + 1 && ep.y <= player.y + 5 && player.iFrameTimer <= 0) {
            player.health--; player.iFrameTimer = 1.5f; ep.isDead = true; if (player.health <= 0) gameOver = true;
        }
    }

    for (const auto& obs : obstacles) {
        if (player.x + 5 > obs.x && player.x < obs.x + obs.width) {
            if (player.y + player.spriteHeight >= obs.y && player.velocityY >= 0 && player.y < obs.y + 2) {
                player.y = obs.y - player.spriteHeight;
                player.velocityY = 0; player.isJumping = false;
            }
        }
    }

    activeProjectiles.erase(std::remove_if(activeProjectiles.begin(), activeProjectiles.end(), [](const Projectile& p){return p.isDead;}), activeProjectiles.end());
    enemyProjectiles.erase(std::remove_if(enemyProjectiles.begin(), enemyProjectiles.end(), [](const Projectile& p){return p.isDead;}), enemyProjectiles.end());
    activeMobs.erase(std::remove_if(activeMobs.begin(), activeMobs.end(), [](const Mob& m){return m.isDead;}), activeMobs.end());
    obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(), [&](const Obstacle& o){ return o.x < -200; }), obstacles.end());
}

void World::drawFrame(const Character& player) {
    std::vector<std::string> grid(height, std::string(width, ' '));
    for (int y = groundLevel; y < height; y++) for (int x = 0; x < width; x++) grid[y][x] = (y == groundLevel) ? '=' : '#';
    for (const auto& obs : obstacles) for (int i = 0; i < obs.height; i++) for (int j = 0; j < obs.width; j++) {
        int ox = (int)obs.x + j, oy = (int)obs.y + i; if (ox >= 0 && ox < width && oy >= 0 && oy < height) grid[oy][ox] = obs.sprite[i][j];
    }
    for (const auto& m : activeMobs) for (int i = 0; i < (int)m.sprite.size(); i++) for (int j = 0; j < (int)m.sprite[i].length(); j++) {
        int mx = (int)m.x + j, my = (int)m.y + i; if (mx >= 0 && mx < width && my >= 0 && my < height) grid[my][mx] = m.sprite[i][j];
    }
    for (const auto& p : activeProjectiles) if (p.x >= 0 && p.x < width) grid[(int)p.y][(int)p.x] = '.';
    for (const auto& ep : enemyProjectiles) if (ep.x >= 0 && ep.x < width) grid[(int)ep.y][(int)ep.x] = '*';

    if (player.iFrameTimer <= 0 || (int)(player.iFrameTimer * 10) % 2 == 0)
        for (int i = 0; i < player.spriteHeight; i++) for (int j = 0; j < player.spriteWidth; j++) {
            int px = (int)player.x + j, py = (int)player.y + i; if (px >= 0 && px < width && py >= 0 && py < height && player.sprite[i][j] != ' ') grid[py][px] = player.sprite[i][j];
        }

    std::string hpStr = "HP: "; for(int i=0; i<3; i++) hpStr += (i<player.health) ? "<3 " : "   ";
    for(int i=0; i<(int)hpStr.length(); i++) if(2<height && 2+i<width) grid[2][2+i] = hpStr[i];
    std::string scStr = "SCORE: " + std::to_string(score);
    for(int i=0; i<(int)scStr.length(); i++) if(2<height && (width/2 - scStr.length()/2)+i<width) grid[2][(width/2 - scStr.length()/2)+i] = scStr[i];
    std::string amStr = isReloading ? "RELOADING..." : "AMMO: " + std::to_string(currentAmmo) + "/" + std::to_string(maxAmmo);
    for(int i=0; i<(int)amStr.length(); i++) if(2<height && (width-amStr.length()-4)+i<width) grid[2][(width-amStr.length()-4)+i] = amStr[i];

    if (gameOver) {
        int midY = height / 2;
        auto drawC = [&](std::string s, int yO) { 
            int stX = (width/2) - (s.length()/2); 
            for(int i=0; i<(int)s.length(); i++) grid[midY+yO][stX+i] = s[i]; 
        };
        for(int dy=-3; dy<=5; dy++) for(int dx=-25; dx<=25; dx++) grid[midY+dy][width/2+dx] = ' ';
        drawC(" G A M E   O V E R ", -1); drawC("FINAL SCORE: " + std::to_string(score), 1); drawC("Press 'SPACE' to Restart", 3);
    }
    std::cout << "\033[H"; for (const auto& l : grid) std::cout << l << "\n"; std::cout << std::flush;
}

void World::startReload(bool manual) { 
    if (isReloading || currentAmmo == maxAmmo) return; 
    isReloading = true; 
    reloadTimer = manual ? 0.8f : 1.0f; 
}

void World::reset(Character& player) {
    resetStateInitialization();
    player.health = 3; player.x = 40; player.y = groundLevel - 6; 
    player.currentState = CharState::RUNNING; player.iFrameTimer = 0;
}