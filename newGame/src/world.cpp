#include "world.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>

World::World(int w, int h) : width(w), height(h) {
    groundLevel = height - 6;
    loadHighScores();
    resetStateInitialization(); 
}

void World::loadHighScores() {
    std::ifstream file("scores.txt");
    if (file.is_open()) {
        file >> highScores[0] >> highScores[1] >> highScores[2];
        file.close();
    }
}

void World::saveHighScores() {
    std::ofstream file("scores.txt");
    if (file.is_open()) {
        file << highScores[0] << " " << highScores[1] << " " << highScores[2];
        file.close();
    }
}

void World::checkHighScore(int finalScore) {
    if (finalScore > highScores[2]) {
        highScores[2] = finalScore;
        // Sort descending so highest is always at index 0
        std::sort(highScores, highScores + 3, std::greater<int>());
        saveHighScores();
    }
}

void World::resetStateInitialization() {
    score = 0; gameOver = false; globalOffset = 0; nextSpawnX = 0;
    lastSegmentType = -1;
    obstacles.clear(); activeMobs.clear(); activeProjectiles.clear(); enemyProjectiles.clear();
    
    // Spawn the first three segments in a fixed order (14, 15, 16)
    spawnSegment(14, 100);
    spawnSegment(15, nextSpawnX);
    spawnSegment(16, nextSpawnX);
    
    // Set lastSegmentType to 16 so the randomizer doesn't accidentally spawn 16 again immediately
    lastSegmentType = 16;
}

void World::spawnSegment(int type, float startX) {
    float segWidth = 130.0f;
    float spawnY = (float)groundLevel - 6;

    switch(type) {
        case 0:
            for(int i=0; i<3; i++) {
                Obstacle h; h.x = startX + (i*50) + 20; h.y = groundLevel - 2;
                h.width = 4; h.height = 2; h.isSolid = true; h.sprite = {"|--|", "|__|"};
                obstacles.push_back(h);
                activeMobs.emplace_back(h.x + 15, spawnY, MobType::CHARGER);
            }
            break;
        case 1:
            for(int i=0; i<2; i++) {
                Obstacle b; b.x = startX + (i*70) + 10; b.y = groundLevel - 12;
                b.width = 30; b.height = 12; b.isSolid = true; b.sprite = std::vector<std::string>(12, std::string(30, '='));
                obstacles.push_back(b);
                activeMobs.emplace_back(b.x + 10, b.y - 6, MobType::ARCHER);
            }
            break;
        case 2:
            for(int i=0; i<3; i++) {
                Obstacle w; w.x = startX + (i*45) + 15; w.y = groundLevel - 5;
                w.width = 4; w.height = 5; w.isSolid = true; w.sprite = {"####","####","####","####","####"};
                obstacles.push_back(w);
                activeMobs.emplace_back(w.x + 8, spawnY, MobType::SENTRY);
            }
            break;
        case 3:
            {
                float leftWallX = startX + 24;
                int wallGap = 5; 
                Obstacle leftWall;
                leftWall.x = leftWallX;
                leftWall.y = (float)groundLevel - 38; 
                leftWall.width = 4;
                leftWall.height = 40 - wallGap; 
                leftWall.isSolid = true;
                leftWall.sprite = std::vector<std::string>(leftWall.height, "||||");
                obstacles.push_back(leftWall);
                Obstacle midPlat;
                midPlat.x = leftWallX + 4;
                midPlat.y = (float)groundLevel - 14; 
                midPlat.width = 15;
                midPlat.height = 1;
                midPlat.isSolid = false;
                midPlat.sprite = {"==============="};
                obstacles.push_back(midPlat);
                float buildX = startX + 90;
                Obstacle build;
                build.x = buildX;
                build.y = (float)groundLevel - 40;
                build.width = 40;
                build.height = 40;
                build.isSolid = true;
                build.sprite = std::vector<std::string>(build.height, std::string(build.width, '|'));
                obstacles.push_back(build);
                Obstacle bPlat1;
                bPlat1.x = buildX - 18;
                bPlat1.y = (float)groundLevel - 22;
                bPlat1.width = 18; bPlat1.height = 1;
                bPlat1.isSolid = false;
                bPlat1.sprite = {"------------------"};
                obstacles.push_back(bPlat1);
                Obstacle bPlat2;
                bPlat2.x = buildX - 18;
                bPlat2.y = (float)groundLevel - 36;
                bPlat2.width = 18; bPlat2.height = 1;
                bPlat2.isSolid = false;
                bPlat2.sprite = {"------------------"};
                obstacles.push_back(bPlat2);
                activeMobs.emplace_back(leftWallX + 10, (float)groundLevel - 5, MobType::SENTRY);
                activeMobs.emplace_back(buildX + 5, (float)groundLevel - 46, MobType::ARCHER);
                break;
            }
        case 4:
            {
                Obstacle spikes;
                spikes.x = startX + 10; 
                spikes.y = (float)groundLevel - 2;
                spikes.width = 100; 
                spikes.height = 2; 
                spikes.isSolid = false; 
                spikes.isHazard = true; 
                spikes.sprite = {"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^", 
                                 "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"};
                obstacles.push_back(spikes);
                for(int i = 0; i < 3; i++) {
                    Obstacle p; 
                    p.x = startX + 20 + (i * 30); 
                    p.y = (float)groundLevel - 5 - (i * 4); 
                    p.width = 11; 
                    p.height = 1; 
                    p.isSolid = false; 
                    p.sprite = {"==========="};
                    obstacles.push_back(p);
                    
                    if(i == 2) {
                        activeMobs.emplace_back(p.x + 2, p.y - 6, MobType::ARCHER);
                    }
                }
                activeMobs.emplace_back(startX + 115, (float)groundLevel - 5, MobType::SENTRY);
                break;
            }

        case 5:
            {
                float center = startX + 60;
                Obstacle pillar; 
                pillar.x = center; 
                pillar.y = (float)groundLevel - 30; 
                pillar.width = 8; 
                pillar.height = 30; 
                pillar.isSolid = true;
                pillar.sprite = std::vector<std::string>(30, "||||||||");
                obstacles.push_back(pillar);
                for(int i = 0; i < 4; i++) {
                    Obstacle plat;
                    plat.width = 12; 
                    plat.height = 1; 
                    plat.isSolid = false;
                    plat.sprite = {"------------"};
                    plat.y = (float)groundLevel - 11 - (i * 5);
                    
                    if(i % 2 == 0) {
                        plat.x = center - 16;
                        if(i == 2) activeMobs.emplace_back(plat.x, plat.y - 5, MobType::SENTRY);
                    } else {
                        plat.x = center + 12;
                        if(i == 3) activeMobs.emplace_back(plat.x + 4, plat.y - 6, MobType::ARCHER);
                    }
                    obstacles.push_back(plat);
                }
                activeMobs.emplace_back(startX + 15, spawnY, MobType::CHARGER);
                break;
            }

        case 6:
            {
                Obstacle roof;
                roof.x = startX + 15; 
                roof.y = (float)groundLevel - 15; 
                roof.width = 90; 
                roof.height = 5; 
                roof.isSolid = true;
                roof.sprite = std::vector<std::string>(5, std::string(90, '='));
                obstacles.push_back(roof);

                for(int i = 0; i < 2; i++) {
                    Obstacle block;
                    block.x = startX + 35 + (i * 40);
                    block.y = (float)groundLevel - 2;
                    block.width = 8; 
                    block.height = 2; 
                    block.isSolid = true;
                    block.isHazard = false; 
                    block.sprite = {"|||||||||", "||||||||"};
                    obstacles.push_back(block);
                    
                    activeMobs.emplace_back(block.x + 15, (float)groundLevel - 5, MobType::SENTRY);
                }
                
                activeMobs.emplace_back(startX + 110, spawnY, MobType::ARCHER);
                break;
            }
        case 7:
            {
                Obstacle pit;
                pit.x = startX + 15; 
                pit.y = (float)groundLevel - 2;
                pit.width = 86; 
                pit.height = 2; 
                pit.isSolid = false; 
                pit.isHazard = true;
                pit.sprite = {"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^",
                              "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"};
                obstacles.push_back(pit);

                for(int i = 0; i < 4; i++) {
                    Obstacle p;
                    p.x = startX + 25 + (i * 20); 
                    p.y = (float)groundLevel - 5;
                    p.width = 5; 
                    p.height = 5; 
                    p.isSolid = true; 
                    p.isHazard = false;
                    p.sprite = {"#####", "#####", "#####", "#####", "#####"};
                    obstacles.push_back(p);
                    
                    if (i % 2 == 1) {
                        activeMobs.emplace_back(p.x, p.y - 6, MobType::ARCHER);
                    }
                }
                break;
            }

        case 8:
            {
                Obstacle t1; 
                t1.x = startX + 15; 
                t1.y = (float)groundLevel - 15;
                t1.width = 40; 
                t1.height = 1; 
                t1.isSolid = false; 
                t1.sprite = {std::string(40, '=')};
                obstacles.push_back(t1);
                Obstacle t2; 
                t2.x = startX + 60; 
                t2.y = (float)groundLevel - 28;
                t2.width = 40; 
                t2.height = 1; 
                t2.isSolid = false; 
                t2.sprite = {std::string(40, '=')};
                obstacles.push_back(t2);
                activeMobs.emplace_back(startX + 25, spawnY, MobType::CHARGER);
                activeMobs.emplace_back(t1.x + 10, t1.y - 5, MobType::SENTRY);
                activeMobs.emplace_back(t2.x + 20, t2.y - 6, MobType::ARCHER);
                activeMobs.emplace_back(startX + 105, spawnY, MobType::CHARGER);
                break;
            }
        case 9:
            {
                Obstacle g1; 
                g1.x = startX; 
                g1.y = (float)groundLevel - 10;
                g1.width = 20; 
                g1.height = 10; 
                g1.isSolid = true;
                g1.sprite = std::vector<std::string>(10, std::string(20, '#'));
                obstacles.push_back(g1);
                Obstacle spikes; 
                spikes.x = startX + 50; 
                spikes.y = (float)groundLevel - 2;
                spikes.width = 20; 
                spikes.height = 2; 
                spikes.isSolid = false; 
                spikes.isHazard = true;
                spikes.sprite = {"^^^^^^^^^^^^^^^^^^^^", "||||||||||||||||||||"};
                obstacles.push_back(spikes);
                Obstacle g2; 
                g2.x = startX + 100; 
                g2.y = (float)groundLevel - 10;
                g2.width = 30; 
                g2.height = 10; 
                g2.isSolid = true;
                g2.sprite = std::vector<std::string>(10, std::string(30, '#'));
                obstacles.push_back(g2);
                activeMobs.emplace_back(startX + 5, g1.y - 5, MobType::SENTRY);
                activeMobs.emplace_back(startX + 75, spawnY, MobType::CHARGER);
                activeMobs.emplace_back(g2.x + 5, g2.y - 6, MobType::ARCHER);
                break;
            }
        case 10:
            {
                Obstacle wall;
                wall.x = startX + 80;
                wall.y = (float)groundLevel - 35;
                wall.width = 15;
                wall.height = 35;
                wall.isSolid = true;
                wall.isHazard = false;
                wall.sprite = std::vector<std::string>(35, std::string(15, '#'));
                obstacles.push_back(wall);
                for(int i = 0; i < 3; i++) {
                    Obstacle plat;
                    plat.x = startX + 20 + (i * 18);
                    plat.y = (float)groundLevel - 10 - (i * 9);
                    plat.width = 15;
                    plat.height = 1;
                    plat.isSolid = false;
                    plat.isHazard = false;
                    plat.sprite = {"==============="};
                    obstacles.push_back(plat);
                }
                activeMobs.emplace_back(startX + 40, spawnY, MobType::SENTRY);
                activeMobs.emplace_back(wall.x + 2, wall.y - 6, MobType::ARCHER);
                break;
            }
        case 11:
            {
                Obstacle leftLedge;
                leftLedge.x = startX;
                leftLedge.y = (float)groundLevel - 13;
                leftLedge.width = 25;
                leftLedge.height = 13;
                leftLedge.isSolid = true;
                leftLedge.sprite = std::vector<std::string>(15, std::string(25, '#'));
                obstacles.push_back(leftLedge);
                Obstacle pit;
                pit.x = startX + 25;
                pit.y = (float)groundLevel - 2;
                pit.width = 65;
                pit.height = 2;
                pit.isSolid = false;
                pit.isHazard = true;
                pit.sprite = {"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^", 
                              "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"};
                obstacles.push_back(pit);
                Obstacle tinyPlat;
                tinyPlat.x = startX + 53;
                tinyPlat.y = (float)groundLevel - 12;
                tinyPlat.width = 6;
                tinyPlat.height = 1;
                tinyPlat.isSolid = false;
                tinyPlat.sprite = {"======"};
                obstacles.push_back(tinyPlat);
                Obstacle rightLedge;
                rightLedge.x = startX + 90;
                rightLedge.y = (float)groundLevel - 15;
                rightLedge.width = 25;
                rightLedge.height = 15;
                rightLedge.isSolid = true;
                rightLedge.sprite = std::vector<std::string>(15, std::string(25, '#'));
                obstacles.push_back(rightLedge);

                break;
            }
        case 12:
            {
                Obstacle highRoad;
                highRoad.x = startX + 10;
                highRoad.y = (float)groundLevel - 13;
                highRoad.width = 100;
                highRoad.height = 1;
                highRoad.isSolid = false;
                highRoad.sprite = {std::string(100, '=')};
                obstacles.push_back(highRoad);
                for(int i = 0; i < 2; i++) {
                    Obstacle spikePatch;
                    spikePatch.x = startX + 30 + (i * 45);
                    spikePatch.y = (float)groundLevel - 2;
                    spikePatch.width = 15;
                    spikePatch.height = 2;
                    spikePatch.isSolid = false;
                    spikePatch.isHazard = true;
                    spikePatch.sprite = {"^^^^^^^^^^^^^^^", "|||||||||||||||"};
                    obstacles.push_back(spikePatch);
                }
                activeMobs.emplace_back(highRoad.x + 20, highRoad.y - 5, MobType::SENTRY);
                activeMobs.emplace_back(highRoad.x + 70, highRoad.y - 5, MobType::SENTRY);
                activeMobs.emplace_back(startX + 15, spawnY, MobType::CHARGER);
                activeMobs.emplace_back(startX + 60, spawnY, MobType::CHARGER);
                break;
            }
        case 13:
            {
                Obstacle pit;
                pit.x = startX + 5;
                pit.y = (float)groundLevel - 2;
                pit.width = 110;
                pit.height = 2;
                pit.isSolid = false;
                pit.isHazard = true;
                pit.sprite = {"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^      ", 
                              "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||      "};
                obstacles.push_back(pit);

                for(int i = 0; i < 4; i++) {
                    Obstacle bridge;
                    bridge.x = startX + 10 + (i * 25);
                    bridge.y = (float)groundLevel - 12; 
                    bridge.width = 14;
                    bridge.height = 1;
                    bridge.isSolid = false;
                    bridge.sprite = {"=============="};
                    obstacles.push_back(bridge);

                    if (i % 2 == 0) {
                        activeMobs.emplace_back(bridge.x + 5, bridge.y - 5, MobType::SENTRY);
                    }
                }
                
                Obstacle endBlock;
                endBlock.x = startX + 115;
                endBlock.y = (float)groundLevel - 12;
                endBlock.width = 8;
                endBlock.height = 12;
                endBlock.isSolid = true;
                endBlock.sprite = std::vector<std::string>(12, "########");
                obstacles.push_back(endBlock);
                
                activeMobs.emplace_back(endBlock.x, endBlock.y - 6, MobType::ARCHER);
                break;
            }
        case 14: // TUTORIAL 1: JUMPS & SPIKES
            {
                Obstacle block1;
                block1.x = startX + 30;
                block1.y = (float)groundLevel - 4;
                block1.width = 4;
                block1.height = 4;
                block1.isSolid = true;
                block1.isHazard = false;
                block1.sprite = std::vector<std::string>(4, "####");
                obstacles.push_back(block1);
                Obstacle spikes;
                spikes.x = startX + 60;
                spikes.y = (float)groundLevel - 2;
                spikes.width = 10;
                spikes.height = 2;
                spikes.isSolid = false;
                spikes.isHazard = true;
                spikes.sprite = {"^^^^^^^^^^", "||||||||||"};
                obstacles.push_back(spikes);
                Obstacle block2;
                block2.x = startX + 90;
                block2.y = (float)groundLevel - 8;
                block2.width = 6;
                block2.height = 8;
                block2.isSolid = true;
                block2.isHazard = false;
                block2.sprite = std::vector<std::string>(8, "######");
                obstacles.push_back(block2);
                break;
            }
        case 15: // TUTORIAL 2: SLIDES & PLATFORMS
            {
                Obstacle roof;
                roof.x = startX + 20;
                roof.y = (float)groundLevel - 16;
                roof.width = 30;
                roof.height = 12;
                roof.isSolid = true;
                roof.isHazard = false;
                roof.sprite = std::vector<std::string>(12, std::string(30, '#'));
                obstacles.push_back(roof);
                Obstacle plat;
                plat.x = startX + 70;
                plat.y = (float)groundLevel - 12;
                plat.width = 20;
                plat.height = 1;
                plat.isSolid = false; 
                plat.isHazard = false;
                plat.sprite = {"===================="};
                obstacles.push_back(plat);
                Obstacle wall;
                wall.x = startX + 90;
                wall.y = (float)groundLevel - 20;
                wall.width = 5;
                wall.height = 20;
                wall.isSolid = true;
                wall.isHazard = false;
                wall.sprite = std::vector<std::string>(20, "#####");
                obstacles.push_back(wall);
                break;
            }
        case 16: // TUTORIAL 3: COMBAT ISOLATION
            {
                activeMobs.emplace_back(startX + 30, spawnY, MobType::CHARGER);
                activeMobs.emplace_back(startX + 70, (float)groundLevel - 5, MobType::SENTRY);
                Obstacle plat;
                plat.x = startX + 110;
                plat.y = (float)groundLevel - 8;
                plat.width = 12;
                plat.height = 1;
                plat.isSolid = false;
                plat.isHazard = false;
                plat.sprite = {"============"};
                obstacles.push_back(plat);
                activeMobs.emplace_back(plat.x + 2, plat.y - 6, MobType::ARCHER);
                break;
            }
        }
        nextSpawnX = startX + segWidth;
}

void World::updateWorld(float deltaTime, Character& player) {
    if (currentState != GameState::PLAYING) return;
    if (gameOver) return;

    // 1. DYNAMIC SPEED SCALING
    // Base speed is 0.5. Every 7 points, we add 10% of that base (0.05).
    float baseSpeed = 0.5f;
    float speedIncrement = 0.05f;
    autoScrollSpeed = baseSpeed + ((score / 7) * speedIncrement);

    // 2. MAP GENERATION
    // We trigger a new spawn when the next segment is about to enter the screen
    if (nextSpawnX - globalOffset < width + 50) {
        int newType;
        do { 
            newType = (rand() % 13) + 1;
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
    if (player.iFrameTimer > 0) player.iFrameTimer -= deltaTime;

    // 3. SOLID WALL, PLATFORM & HAZARD LOGIC
    for (auto& obs : obstacles) {
        obs.x -= currentSpeed;
        
        float pTop = (player.currentState == CharState::CROUCHING) ? player.y + 3 : player.y;
        float pBottom = player.y + 6;
        float pLeft = player.x, pRight = player.x + player.spriteWidth;
        
        float oTop = obs.y, oBottom = obs.y + obs.height;
        float oLeft = obs.x, oRight = obs.x + obs.width;

        // Hazard Collision (Spikes)
        if (obs.isHazard) {
            if (pBottom > oTop && pTop < oBottom && pRight > oLeft && pLeft < oRight) {
                if (player.iFrameTimer <= 0) {
                    player.health--;
                    player.iFrameTimer = 1.5f;
                    if (player.health <= 0) gameOver = true;
                }
            }
        }

        // Solid Collision (Walls & Ceilings)
        if (obs.isSolid) {
            if (pBottom > oTop && pTop < oBottom && pRight > oLeft && pLeft < oRight) {
                float overlapLeft = pRight - oLeft;
                float overlapRight = oRight - pLeft;
                float overlapTop = oBottom - pTop;

                // Ceiling Collision: Hitting head while jumping up
                if (player.velocityY < 0 && overlapTop < 4.0f && overlapLeft > 2.0f && overlapRight > 2.0f) {
                    player.y = oBottom; 
                    if (player.currentState == CharState::CROUCHING) player.y -= 3;
                    player.velocityY = 0; // Kill upward momentum
                }
                // Horizontal resolution
                else if (pBottom > oTop + 1.5f) { 
                    if (overlapLeft < overlapRight && overlapLeft < 6.0f) {
                        player.x = oLeft - player.spriteWidth; // Pushed left
                    } 
                    else if (overlapRight < overlapLeft && overlapRight < 6.0f) {
                        player.x = oRight; // Pushed right
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
                if (player.x < m.x && player.x > m.x - 10.0f && std::abs(player.y - m.y) < 3 && player.iFrameTimer <= 0) {
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
                if (m.health <= 0) { 
                    m.isDead = true; 
                    score++; 
                    // Grant ammo on kill
                    if (player.currentAmmo < player.maxAmmo) {
                        player.currentAmmo++;
                    }
                }
            }
        }
        
        // Melee Attack vs Mob
        if (player.isMeleeAttacking) {
            float spearRange = 10.0f;
            
            // Tighter vertical hitbox
            float hitboxTop = player.y + 2.0f;
            float hitboxBottom = player.y + 5.0f;
            
            float spearStartX = player.facingRight ? player.x + 9 : player.x - spearRange;
            float spearEndX = player.facingRight ? player.x + 9 + spearRange : player.x;
            
            // Check collision: Does the spear's horizontal line intersect the mob's body?
            if (m.x <= spearEndX && m.x + m.width >= spearStartX &&
                m.y + m.height > hitboxTop && m.y < hitboxBottom) {
                m.health--; 
                if (m.health <= 0) { 
                    m.isDead = true; 
                    score++; 
                    // Grant ammo on kill
                    if (player.currentAmmo < player.maxAmmo) {
                        player.currentAmmo++;
                    }
                }
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

    if (gameOver) {
        currentState = GameState::GAMEOVER;
        checkHighScore(score);
    }
}

void World::drawFrame(const Character& player) {
    // 1. Initialize the grid
    std::vector<std::string> grid(height, std::string(width, ' '));

    auto safeSet = [&](int x, int y, char c) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = c;
        }
    };

    // Helper lambda for centered text (used for Menus & Game Over)
    auto drawCentered = [&](std::string s, int offset) {
        int midY = height / 2;
        int midX = width / 2;
        int startX = midX - ((int)s.length() / 2);
        for(int i = 0; i < (int)s.length(); i++) {
            safeSet(startX + i, midY + offset, s[i]);
        }
    };

    // ==========================================
    // MENU SCREENS
    // ==========================================
    if (currentState == GameState::MENU) {
        drawCentered("=======================================", -6);
        drawCentered("         TERMINAL SHOWDOWN         ", -4);
        drawCentered("=======================================", -2);
        drawCentered("Press [SPACE] to Start Game", 2);
        drawCentered("Press [S] for High Scores", 4);
        drawCentered("Press [Q] to Quit", 6);
    } 
    else if (currentState == GameState::HIGHSCORES) {
        drawCentered("=== TOP 3 SCORES ===", -4);
        drawCentered("1st Place: " + std::to_string(highScores[0]), -1);
        drawCentered("2nd Place: " + std::to_string(highScores[1]), +1);
        drawCentered("3rd Place: " + std::to_string(highScores[2]), +3);
        drawCentered("Press [M] to Return to Menu", 7);
    } 
    // ==========================================
    // GAMEPLAY & GAME OVER SCREENS
    // ==========================================
    else {
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
        
        // 6.5 Draw Melee Attack Animation
        if (player.isMeleeAttacking) {
            float progress = 1.0f - (player.meleeAttackTimer / 0.3f);
            int frame = (int)(progress * 10);
            if (frame > 9) frame = 9;
            
            int axeX = (int)player.x + 7; // Right of character
            int axeY = (int)player.y;
            
            std::vector<std::string> axeFrames[] = {
                {"        ", "        ", "--     ", "        ", "        ", "        ", "        ", "        "},
                {"        ", "        ", "-----  ", "        ", "        ", "        ", "        ", "        "},
                {"        ", "        ", "------->", "        ", "        ", "        ", "        ", "        "},
                {"        ", "        ", "---------->", "        ", "        ", "        ", "        ", "        "},
                {"        ", "        ", "----------->", "        ", "        ", "        ", "        ", "        "},
                {"        ", "        ", "---------->", "        ", "        ", "        ", "        ", "        "},
                {"        ", "        ", "------->", "        ", "        ", "        ", "        ", "        "},
                {"        ", "        ", "-----  ", "        ", "        ", "        ", "        ", "        "},
                {"        ", "        ", "--     ", "        ", "        ", "        ", "        ", "        "},
                {"        ", "        ", "       ", "        ", "        ", "        ", "        ", "        "}
            };
            
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < (int)axeFrames[frame][i].length(); j++) {
                    if (axeFrames[frame][i][j] != ' ') {
                        safeSet(axeX + j, axeY + i, axeFrames[frame][i][j]);
                    }
                }
            }
        }

        // 7. Draw HUD 
        // HP Display
        std::string hpStr = "HP: "; 
        for(int i=0; i<3; i++) hpStr += (i < player.health) ? "<3 " : "   ";
        for(int i=0; i<(int)hpStr.length(); i++) safeSet(2 + i, 2, hpStr[i]);

        // Score Display
        std::string scStr = "SCORE: " + std::to_string(score);
        int scX = (width / 2) - (scStr.length() / 2);
        for(int i=0; i<(int)scStr.length(); i++) safeSet(scX + i, 2, scStr[i]);

        // Ammo Display
        std::string amStr = "AMMO: " + std::to_string(player.currentAmmo) + "/" + std::to_string(player.maxAmmo);
        int amX = width - (int)amStr.length() - 4;
        for(int i=0; i<(int)amStr.length(); i++) safeSet(amX + i, 2, amStr[i]);
        
        // Melee Cooldown Display
        if (player.meleeCooldownTimer > 0) {
            std::string meleeStr = "E: [COOLDOWN]";
            int meleeX = 2;
            for(int i=0; i<(int)meleeStr.length(); i++) safeSet(meleeX + i, 3, meleeStr[i]);
        } else {
            std::string meleeStr = "E: MELEE READY";
            int meleeX = 2;
            for(int i=0; i<(int)meleeStr.length(); i++) safeSet(meleeX + i, 3, meleeStr[i]);
        }

        // 8. Game Over Overlay (Now checks currentState)
        if (currentState == GameState::GAMEOVER) {
            int midY = height / 2;
            int midX = width / 2;
            
            // Clear a box so text is readable over the background
            for(int dy = -4; dy <= 6; dy++) {
                for(int dx = -20; dx <= 20; dx++) {
                    safeSet(midX + dx, midY + dy, ' ');
                }
            }

            drawCentered(" G A M E   O V E R ", -2);
            drawCentered("FINAL SCORE: " + std::to_string(score), 0);
            drawCentered("Press [SPACE] to Restart", 2);
            drawCentered("Press [M] for Main Menu", 4);
        }
    }

    // 9. Render to Terminal
    std::cout << "\033[H"; 
    for (const auto& line : grid) {
        std::cout << line << "\n";
    }
    std::cout << std::flush;
}

void World::reset(Character& player) {
    resetStateInitialization();
    player.health = 3; 
    player.x = 40; 
    player.y = groundLevel - 6; 
    player.currentState = CharState::RUNNING; 
    player.iFrameTimer = 0;
    player.currentAmmo = player.maxAmmo;
}