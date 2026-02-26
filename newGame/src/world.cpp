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
    float segWidth = 130.0f;
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
        case 3: // THE SLIDE & CLIMB
            {
                float leftWallX = startX + 24;
                int wallGap = 5; // Gap for sliding
                
                // 1. LEFT WALL
                Obstacle leftWall;
                leftWall.x = leftWallX;
                leftWall.y = (float)groundLevel - 38; 
                leftWall.width = 4;
                leftWall.height = 40 - wallGap; 
                leftWall.isSolid = true;
                leftWall.sprite = std::vector<std::string>(leftWall.height, "||||");
                obstacles.push_back(leftWall);

                // 2. MID-PLATFORM (Attached to the right of the left wall)
                Obstacle midPlat;
                midPlat.x = leftWallX + 4;
                midPlat.y = (float)groundLevel - 14; // Mid-height
                midPlat.width = 15;
                midPlat.height = 1;
                midPlat.isSolid = false; // Semi-solid platform
                midPlat.sprite = {"==============="};
                obstacles.push_back(midPlat);

                // 3. RIGHT BUILDING
                float buildX = startX + 90;
                Obstacle build;
                build.x = buildX;
                build.y = (float)groundLevel - 40;
                build.width = 40;
                build.height = 40;
                build.isSolid = true;
                build.sprite = std::vector<std::string>(build.height, std::string(build.width, '|'));
                obstacles.push_back(build);

                // 4. TWO PLATFORMS ON BUILDING'S LEFT
                // Lower Platform (Reach from Mid-Platform)
                Obstacle bPlat1;
                bPlat1.x = buildX - 18;
                bPlat1.y = (float)groundLevel - 22;
                bPlat1.width = 18; bPlat1.height = 1;
                bPlat1.sprite = {"------------------"};
                obstacles.push_back(bPlat1);

                // Upper Platform
                Obstacle bPlat2;
                bPlat2.x = buildX - 18;
                bPlat2.y = (float)groundLevel - 36;
                bPlat2.width = 18; bPlat2.height = 1;
                bPlat2.sprite = {"------------------"};
                obstacles.push_back(bPlat2);

                // Mobs: Spearman under the left wall, Archer on building top
                activeMobs.emplace_back(leftWallX + 10, (float)groundLevel - 5, MobType::SENTRY);
                activeMobs.emplace_back(buildX + 5, (float)groundLevel - 46, MobType::ARCHER);
                break;
            }
        }
        nextSpawnX = startX + segWidth;
}

void World::updateWorld(float deltaTime, Character& player) {
    if (gameOver) return;

    // 1. DYNAMIC SPEED SCALING
    // Base speed is 0.5. Every 7 points, we add 10% of that base (0.05).
    float baseSpeed = 0.5f;
    float speedIncrement = 0.05f;
    autoScrollSpeed = baseSpeed + ((score / 7) * speedIncrement);

    // 2. MAP GENERATION (Procedural Stitching)
    // We trigger a new spawn when the next segment is about to enter the screen
    if (nextSpawnX - globalOffset < width + 50) {
        int newType;
        do { 
            newType = rand() % 5; 
        } while (newType == lastSegmentType);
        
        lastSegmentType = newType;
        spawnSegment(newType, nextSpawnX);
    }
    
    // Movement speed including the "Pushing Right" boost
    float currentSpeed = player.isPushingRight ? (autoScrollSpeed * 3.0f) : autoScrollSpeed;
    globalOffset += currentSpeed;

    // Death check: If pushed off-screen left
    if (player.x + player.spriteWidth <= 0) { gameOver = true; return; }
    
    // Status Timers
    if (isReloading) { 
        reloadTimer -= deltaTime; 
        if (reloadTimer <= 0) { currentAmmo = maxAmmo; isReloading = false; } 
    }
    if (player.iFrameTimer > 0) player.iFrameTimer -= deltaTime;

    // 3. SOLID WALL & PLATFORM LOGIC (Directional Collision)
    for (auto& obs : obstacles) {
        obs.x -= currentSpeed;
        
        if (obs.isSolid) {
            // Hitbox definitions
            float pTop = (player.currentState == CharState::CROUCHING) ? player.y + 3 : player.y;
            float pBottom = player.y + 6;
            float pLeft = player.x, pRight = player.x + player.spriteWidth;
            
            float oTop = obs.y, oBottom = obs.y + obs.height;
            float oLeft = obs.x, oRight = obs.x + obs.width;

            // AABB Overlap check
            if (pBottom > oTop && pTop < oBottom && pRight > oLeft && pLeft < oRight) {
                float overlapLeft = pRight - oLeft;
                float overlapRight = oRight - pLeft;

                // Horizontal resolution (only if not landing on top)
                if (pBottom > oTop + 1.5f) { 
                    // Pushed from Left side
                    if (overlapLeft < overlapRight && overlapLeft < 6.0f) {
                        player.x = oLeft - player.spriteWidth;
                    } 
                    // Pushed from Right side
                    else if (overlapRight < overlapLeft && overlapRight < 6.0f) {
                        player.x = oRight;
                    }
                }
            }
        }

        // ONE-WAY LANDING
        if (player.velocityY >= 0) {
            bool withinX = (player.x + 6 > obs.x && player.x + 2 < obs.x + obs.width);
            float feet = player.y + 6;
            
            if (withinX && feet >= obs.y && feet <= obs.y + 2.5f) {
                player.y = obs.y - 6;
                player.velocityY = 0;
                player.isJumping = false;
            }
        }
    }

    // 4. PROJECTILE UPDATES
    // Player Bullets (Left-to-Right)
    for (auto& p : activeProjectiles) {
        float oldX = p.x;
        p.x -= currentSpeed; 
        p.update(width); 
        float newX = p.x;

        for (const auto& obs : obstacles) {
            if (obs.isSolid && p.y >= obs.y && p.y <= obs.y + obs.height) {
                if (oldX < obs.x && newX >= obs.x) {
                    p.isDead = true; 
                    p.x = obs.x; 
                }
            }
        }
    }

    // Enemy Arrows (Right-to-Left, checking tip against RIGHT wall boundary)
    for (auto& ep : enemyProjectiles) {
        float oldX = ep.x;
        ep.x -= currentSpeed; 
        ep.update(width);
        float newX = ep.x;

        for (const auto& obs : obstacles) {
            if (obs.isSolid && ep.y >= obs.y && ep.y <= obs.y + obs.height) {
                float wallBoundary = obs.x + obs.width;
                if (oldX > wallBoundary && newX <= wallBoundary) {
                    ep.isDead = true; 
                    ep.x = wallBoundary; 
                }
            }
        }
    }

    // 5. MOB LOGIC (AI & Combat)
    for (auto& m : activeMobs) {
        m.x -= currentSpeed; 
        m.update(deltaTime, player.x, player.y);

        // Spearman Thrust
        if (m.type == MobType::SENTRY) {
            int frame = (int)(m.animTimer * 10) % 20;
            if (frame >= 10) { 
                if (player.x < m.x && player.x > m.x - 14.0f && std::abs(player.y - m.y) < 4 && player.iFrameTimer <= 0) {
                    player.health--; 
                    player.iFrameTimer = 1.5f;
                    if (player.health <= 0) gameOver = true;
                }
            }
        }

        // Archer Shoot Logic
        if (m.type == MobType::ARCHER && m.shootTimer > 2.2f) { 
            enemyProjectiles.emplace_back(m.x, m.y + 1, -2.5f); 
            m.shootTimer = 0; 
        }

        // General Body Collision
        if (std::abs(player.x - m.x) < 4 && std::abs(player.y - m.y) < 3 && player.iFrameTimer <= 0) {
            player.health--; 
            player.iFrameTimer = 1.5f; 
            if (player.health <= 0) gameOver = true;
        }

        // Bullet vs Mob
        for (auto& p : activeProjectiles) {
            if (!p.isDead && p.x >= m.x && p.x <= m.x + m.width && p.y >= m.y && p.y <= m.y + m.height) {
                m.health--; 
                p.isDead = true;
                if (m.health <= 0) { m.isDead = true; score++; }
            }
        }
    }

    // 6. PLAYER HIT DETECTION & CLEANUP
    for (auto& ep : enemyProjectiles) {
        if (!ep.isDead && ep.x >= player.x + 2 && ep.x <= player.x + 7 && ep.y >= player.y + 1 && ep.y <= player.y + 5 && player.iFrameTimer <= 0) {
            player.health--; 
            player.iFrameTimer = 1.5f; 
            ep.isDead = true; 
            if (player.health <= 0) gameOver = true;
        }
    }

    activeProjectiles.erase(std::remove_if(activeProjectiles.begin(), activeProjectiles.end(), [](const Projectile& p){return p.isDead;}), activeProjectiles.end());
    enemyProjectiles.erase(std::remove_if(enemyProjectiles.begin(), enemyProjectiles.end(), [](const Projectile& ep){return ep.isDead;}), enemyProjectiles.end());
    activeMobs.erase(std::remove_if(activeMobs.begin(), activeMobs.end(), [](const Mob& m){return m.isDead;}), activeMobs.end());
    obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(), [&](const Obstacle& o){ return o.x < -200; }), obstacles.end());
}

void World::drawFrame(const Character& player) {
    // 1. Initialize the grid
    std::vector<std::string> grid(height, std::string(width, ' '));

    // Helper lambda for absolute safety - prevents SegFaults by catching out-of-bounds coords
    auto safeSet = [&](int x, int y, char c) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = c;
        }
    };

    // 2. Draw Ground and Under-ground
    for (int y = groundLevel; y < height; y++) {
        for (int x = 0; x < width; x++) {
            safeSet(x, y, (y == groundLevel) ? '=' : '#');
        }
    }

    // 3. Draw Obstacles (Buildings/Platforms)
    for (const auto& obs : obstacles) {
        for (int i = 0; i < obs.height; i++) {
            for (int j = 0; j < obs.width; j++) {
                // Bounds checking via safeSet prevents crashes if obstacles are partially off-screen
                safeSet((int)obs.x + j, (int)obs.y + i, obs.sprite[i][j]);
            }
        }
    }

    // 4. Draw Mobs
    for (const auto& m : activeMobs) {
        for (int i = 0; i < (int)m.sprite.size(); i++) {
            for (int j = 0; j < (int)m.sprite[i].length(); j++) {
                safeSet((int)m.x + j, (int)m.y + i, m.sprite[i][j]);
            }
        }
    }

    // 5. Draw Projectiles
    for (const auto& p : activeProjectiles) {
        safeSet((int)p.x, (int)p.y, '.');
    }
    
    for (const auto& ep : enemyProjectiles) {
        std::string arrowSprite = "<---";
        for (int i = 0; i < (int)arrowSprite.length(); i++) {
            // We draw the arrow starting from ep.x and moving to the right
            safeSet((int)ep.x + i, (int)ep.y, arrowSprite[i]);
        }
    }

    // 6. Draw Player
    if (player.iFrameTimer <= 0 || (int)(player.iFrameTimer * 10) % 2 == 0) {
        for (int i = 0; i < player.spriteHeight; i++) {
            for (int j = 0; j < player.spriteWidth; j++) {
                if (player.sprite[i][j] != ' ') {
                    safeSet((int)player.x + j, (int)player.y + i, player.sprite[i][j]);
                }
            }
        }
    }

    // 7. Draw HUD (Strict Bounds Checking for terminal resizing safety)
    // HP Display
    std::string hpStr = "HP: "; 
    for(int i=0; i<3; i++) hpStr += (i < player.health) ? "<3 " : "   ";
    for(int i=0; i<(int)hpStr.length(); i++) safeSet(2 + i, 2, hpStr[i]);

    // Score Display
    std::string scStr = "SCORE: " + std::to_string(score);
    int scX = (width / 2) - (scStr.length() / 2);
    for(int i=0; i<(int)scStr.length(); i++) safeSet(scX + i, 2, scStr[i]);

    // Ammo/Reloading Display
    std::string amStr = isReloading ? "RELOADING..." : "AMMO: " + std::to_string(currentAmmo) + "/" + std::to_string(maxAmmo);
    int amX = width - (int)amStr.length() - 4;
    for(int i=0; i<(int)amStr.length(); i++) safeSet(amX + i, 2, amStr[i]);

    // 8. Game Over Overlay
    if (gameOver) {
        int midY = height / 2;
        int midX = width / 2;
        
        for(int dy = -3; dy <= 5; dy++) {
            for(int dx = -20; dx <= 20; dx++) {
                safeSet(midX + dx, midY + dy, ' ');
            }
        }

        auto drawCentered = [&](std::string s, int offset) {
            int startX = midX - ((int)s.length() / 2);
            for(int i = 0; i < (int)s.length(); i++) {
                safeSet(startX + i, midY + offset, s[i]);
            }
        };

        drawCentered(" G A M E   O V E R ", -1);
        drawCentered("FINAL SCORE: " + std::to_string(score), 1);
        drawCentered("Press 'SPACE' to Restart", 3);
    }

    // 9. Render to Terminal
    std::cout << "\033[H"; 
    for (const auto& line : grid) {
        std::cout << line << "\n";
    }
    std::cout << std::flush;
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