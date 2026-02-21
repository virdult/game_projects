#include "world.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

World::World(int w, int h) : width(w), height(h) {
    groundLevel = height - 6;
    generateWorld();
}

void World::addObject(int x, int y, std::vector<std::string> sprite, int layer) {
    BackgroundObject obj;
    obj.x = (float)x;
    obj.y = y;
    obj.layer = layer;
    size_t maxWidth = 0;
    for (const auto& s : sprite) if (s.length() > maxWidth) maxWidth = s.length();
    obj.sprite = sprite;
    for (auto& s : obj.sprite) {
        if (s.length() < maxWidth) s.append(maxWidth - s.length(), ' ');
    }
    obj.width = (int)maxWidth;
    obj.height = (int)obj.sprite.size();
    bgObjects.push_back(obj);
}

void World::updateWorld(float deltaTime, float playerX, float playerY, float playerVelX) {
    // 1. Update scrolling and bullets
    updateProjectiles(deltaTime, playerVelX);

    // 2. Update Mobs with BOTH X and Y
    for (auto& m : activeMobs) {
        // Pass playerY here so the Flyer knows where you are to dive!
        m.update(deltaTime, playerX, playerY);

        // 3. Collision Check: Projectile vs Mob
        for (auto& p : activeProjectiles) {
            if (!p.isDead && p.x >= m.x && p.x <= m.x + m.width && 
                p.y >= m.y && p.y <= m.y + m.height) {
                m.health--;
                p.isDead = true;
                if (m.health <= 0) m.isDead = true;
            }
        }
    }

    // 4. Cleanup dead mobs
    activeMobs.erase(std::remove_if(activeMobs.begin(), activeMobs.end(),
        [](const Mob& m) { return m.isDead; }), activeMobs.end());
}

void World::generateWorld() {
    int vWidth = 2000;

    // --- LAYER 0: THE SHINING SUN (Celestial - Speed 0.01) ---
    addObject(160, 2, {
        "    \\  |  /    ",
        "     .-''-.     ",
        "  -- (    ) --  ",
        "     '-..-'     ",
        "    /  |  \\    "
    }, 0);

    // --- LAYER 1: FAR HORIZON (Speed 0.05) ---
    for (int x = 0; x < vWidth; x += 300) {
        addObject(x, 3, {" ( ) "}, 1); // Cloud Type 1: Tiny Puff
        addObject(x + 150, 5, {"_(_)_"}, 1); // Cloud Type 2: Flat Drifter
    }

    // --- LAYER 2: THE METROPOLIS (Speed 0.15) ---
    for (int x = 0; x < vWidth; x += 350) {
        // --- 10-FLOOR MEGA SKYSCRAPER ---
        std::vector<std::string> tower;
        tower.push_back("   .-------------.   ");
        tower.push_back("   |  _   _   _  |   ");
        // Generate 10 floors of windows
        for(int f = 0; f < 10; f++) {
            tower.push_back("   | [ ] [ ] [ ] |   ");
            tower.push_back("   | [ ] [ ] [ ] |   ");
        }
        tower.push_back("   |_____________|   ");

        // Calculate height for road connection
        // Tower is 2 + (10*2) + 1 = 23 lines tall.
        // Starting at y = 22 puts the top below the main cloud line.
        int currentTowerHeight = tower.size();
        int roadLength = (groundLevel - 22 - currentTowerHeight);
        
        addObject(x, 22, tower, 2); 

        addObject(x + 120, 10, {" .----. ", "(      )", " '----' "}, 2); // Cloud Type 3: Puffy
    }

    // --- LAYER 3: MID-GROUND (Small Mansion & Baby Trees - Speed 0.35) ---
    for (int x = 50; x < vWidth; x += 200) {
        // SMALL MANSION
        addObject(x, groundLevel - 4, {
            "   ______   ",
            "  /      \\  ",
            " |  [][]  | ",
            " |___||___| "
        }, 3);

        // BABY TREE
        addObject(x + 120, groundLevel - 5, {
            "  /\\  ",
            " /##\\ ",
            "  ||  "
        }, 3);

        addObject(x + 60, 14, {"  _(    )_  ", " (________) "}, 3); // Cloud Type 4: Wide
    }

    // --- LAYER 4: BACK BACKGROUND (Cottages & Mid-Trees - Speed 0.7) ---
    for (int x = 20; x < vWidth; x += 180) {
        addObject(x, groundLevel - 3, {"  ____  ", " /    \\ ", "|_[__]_|"}, 4);
        
        addObject(x + 100, groundLevel - 6, {
            "    /\\    ",
            "   /##\\   ",
            "  /####\\  ",
            "    ||    "
        }, 4);

        addObject(x + 40, 17, {" (__________) ", "  '--------'  "}, 4); // Cloud Type 5: Low Long
    }

    // --- LAYER 5: FOREGROUND (Large Mansions & Massive Trees - Speed 1.0) ---
    for (int x = 100; x < vWidth; x += 350) {
        // Cloud Type 6: Massive Storm Cloud
        addObject(x, 8, {
            "             .--------.             ",
            "         ___(          )___         ",
            "      __(                  )__      ",
            "     (________________________)     "
        }, 5);

        // BIG MANSION (Grounded)
        addObject(x + 60, groundLevel - 6, {
            "      __________      ",
            "   __/          \\__   ",
            "  /                \\  ",
            " |   [][]    [][]   | ",
            " |        __        | ",
            " |_______|  |_______| "
        }, 5);

        // MASSIVE CHUNKY TREE
        addObject(x + 220, groundLevel - 13, {
            "       /\\       ",
            "      /##\\      ",
            "     /####\\     ",
            "    /######\\    ",
            "   /########\\   ",
            "  /##########\\  ",
            " /############\\ ",
            "      ||||      ",
            "      ||||      ",
            "      ||||      ",
            "      ||||      "
        }, 5);
    }
}

void World::updateProjectiles(float deltaTime, float playerVelX) {
    if (cooldownTimer > 0) cooldownTimer -= deltaTime;
    if (isReloading) {
        reloadTimer -= deltaTime;
        if (reloadTimer <= 0) { currentAmmo = maxAmmo; isReloading = false; }
    }
    globalOffset += playerVelX;
    for (auto& p : activeProjectiles) p.update(width);
}

void World::startReload(bool manual) {
    if (isReloading || currentAmmo == maxAmmo) return;
    isReloading = true;
    reloadTimer = manual ? 0.8f : 1.0f;
}

void World::drawFrame(const Character& player) {
    // 1. Initialize a clean grid
    std::vector<std::string> grid(height, std::string(width, ' '));
    const int vWidth = 2000;

    // 2. Draw 6 Parallax Layers (0 to 5) - Background First
    for (int layer = 0; layer <= 5; layer++) {
        float speed;
        switch(layer) {
            case 5: speed = 1.00f; break; // Foreground
            case 4: speed = 0.70f; break; // Small stuff
            case 3: speed = 0.40f; break; // Mid Clouds
            case 2: speed = 0.20f; break; // City + Roads
            case 1: speed = 0.05f; break; // Far Sky
            case 0: speed = 0.01f; break; // Sun
            default: speed = 1.0f;
        }

        for (const auto& obj : bgObjects) {
            if (obj.layer != layer) continue;
            int screenX = ((int)(obj.x - (globalOffset * speed))) % vWidth;
            if (screenX < 0) screenX += vWidth;

            for (int dy = 0; dy < obj.height; dy++) {
                for (int dx = 0; dx < obj.width; dx++) {
                    int finalX = screenX + dx;
                    int finalY = obj.y + dy;
                    if (finalY >= 0 && finalY < height && finalX >= 0 && finalX < width) {
                        if (obj.sprite[dy][dx] != ' ') grid[finalY][finalX] = obj.sprite[dy][dx];
                    }
                    // Wrapping for infinite scroll
                    int wrapX = finalX - vWidth;
                    if (finalY >= 0 && finalY < height && wrapX >= 0 && wrapX < width) {
                        if (obj.sprite[dy][dx] != ' ') grid[finalY][wrapX] = obj.sprite[dy][dx];
                    }
                }
            }
        }
    }

    // 3. Draw Ground (Drawn over background)
    for (int y = groundLevel; y < height; y++) {
        for (int x = 0; x < width; x++) grid[y][x] = (y == groundLevel) ? '=' : '#';
    }

    // 4. Draw Player (Highest Priority - Always visible)
    // Make sure player.y is correctly updated in main.cpp to groundLevel - spriteHeight
    int py = (int)player.y, px = (int)player.x;
    for (int i = 0; i < player.spriteHeight; i++) {
        for (int j = 0; j < player.spriteWidth; j++) {
            if (py + i >= 0 && py + i < height && px + j >= 0 && px + j < width) {
                if (player.sprite[i][j] != ' ') grid[py + i][px + j] = player.sprite[i][j];
            }
        }
    }
    //MOB
    for (const auto& m : activeMobs) {
        for (int i = 0; i < m.height; i++) {
            for (int j = 0; j < m.width; j++) {
                int my = (int)m.y + i;
                int mx = (int)m.x + j;
                if (my >= 0 && my < height && mx >= 0 && mx < width) {
                    if (m.sprite[i][j] != ' ') grid[my][mx] = m.sprite[i][j];
                }
            }
        }
    }

    // 5. Draw UI & Projectiles
    std::string ammoStr = isReloading ? "RELOADING..." : "AMMO: " + std::to_string(currentAmmo) + "/8";
    int uiX = width - (int)ammoStr.length() - 2;
    if (uiX > 0) {
        for (int i = 0; i < (int)ammoStr.length(); i++) grid[2][uiX + i] = ammoStr[i];
    }
    for (const auto& p : activeProjectiles) {
        if ((int)p.y >= 0 && (int)p.y < height && (int)p.x >= 0 && (int)p.x < width) 
            grid[(int)p.y][(int)p.x] = p.symbol;
    }

    // 6. Flush to terminal
    std::string output = "\033[H";
    for (const auto& line : grid) output += line + "\n";
    std::cout << output << std::flush;
}