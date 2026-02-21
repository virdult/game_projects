#include "world.hpp"
#include "character.hpp"
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Input", 0, 0, 1, 1, SDL_WINDOW_SHOWN);
    
    World gameWorld(204, 55);
    Character player(10.0f, gameWorld.getGroundLevel() - 6);
    
    bool isRunning = true;
    SDL_Event e;
    const float frameTime = 0.030f; 

    while (isRunning) {
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) isRunning = false;

        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_Q]) isRunning = false;

        bool moveLeft = state[SDL_SCANCODE_A];
        bool moveRight = state[SDL_SCANCODE_D];
        bool isCrouching = state[SDL_SCANCODE_S];
        if (state[SDL_SCANCODE_W]) player.jump();
        if (state[SDL_SCANCODE_R]) gameWorld.startReload(true);

        // Fixed Shooting Logic
        if (state[SDL_SCANCODE_SPACE]) {
            if (!gameWorld.isReloading && gameWorld.currentAmmo > 0 && gameWorld.cooldownTimer <= 0) {
                float bulletVel = player.facingRight ? 6.0f : -6.0f;
                float spawnX = player.facingRight ? player.x + 7 : player.x + 1;
                float spawnY = (player.currentState == CharState::CROUCHING) ? player.y + 3 : player.y + 1;
                
                gameWorld.activeProjectiles.emplace_back(spawnX, spawnY, bulletVel);
                gameWorld.currentAmmo--;
                gameWorld.cooldownTimer = 0.15f;
                if (gameWorld.currentAmmo <= 0) gameWorld.startReload(false);
            }
        }

        player.update(gameWorld.getGroundLevel(), 204, moveLeft, moveRight, isCrouching);
        gameWorld.updateProjectiles(frameTime, player.velocityX);
        gameWorld.drawFrame(player);

        static float spawnTimer = 0;
        spawnTimer += frameTime;

        if (spawnTimer > 4.0f) { // Spawning slightly slower too
        bool fromRight = (rand() % 2 == 0);
        float spawnX = fromRight ? 220 : -20; // Spawn further out
        
        int r = rand() % 3;
        MobType t = (r == 0) ? MobType::CRAWLER : (r == 1 ? MobType::WALKER : MobType::FLYER);
        
        float spawnY;
        if (t == MobType::FLYER) {
            // Lowered for Jump-Shooting: roughly middle of the sky
            spawnY = gameWorld.getGroundLevel() - 12; 
        } else if (t == MobType::WALKER) {
            spawnY = gameWorld.getGroundLevel() - 5; // Fixed 1-tile-up position
        } else {
            spawnY = gameWorld.getGroundLevel() - 2;
        }
        
        gameWorld.activeMobs.emplace_back(spawnX, spawnY, t);
        spawnTimer = 0;
    }

    // Update the world call
    gameWorld.updateWorld(frameTime, player.x, player.y, player.velocityX);

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    SDL_Quit();
    return 0;
}