#include "world.hpp"
#include <SDL2/SDL.h>
#include <thread>
#include <ctime>

int main() {
    srand(static_cast<unsigned int>(time(NULL)));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Input", 0, 0, 1, 1, SDL_WINDOW_SHOWN);
    World gameWorld(204, 55);
    Character player(40.0f, gameWorld.getGroundLevel() - 6);
    bool isRunning = true; SDL_Event e; const float fTime = 0.030f;

    while (isRunning) {
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) isRunning = false;
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_Q]) isRunning = false;

        if (!gameWorld.gameOver) {
            bool mL = state[SDL_SCANCODE_A], mR = state[SDL_SCANCODE_D], iC = state[SDL_SCANCODE_S];
            if (state[SDL_SCANCODE_W]) player.jump();
            if (state[SDL_SCANCODE_SPACE] && gameWorld.cooldownTimer <= 0 && !gameWorld.isReloading && gameWorld.currentAmmo > 0) {
                float shotY = iC ? (player.y + 4) : (player.y + 2);
                gameWorld.activeProjectiles.emplace_back(player.x + 7, shotY, 4.0f);
                gameWorld.currentAmmo--; gameWorld.cooldownTimer = 0.20f;
                if (gameWorld.currentAmmo <= 0) gameWorld.startReload(false);
            }
            if (state[SDL_SCANCODE_R]) gameWorld.startReload(true);
            player.update(gameWorld.getGroundLevel(), 204, mL, mR, iC);
            gameWorld.updateWorld(fTime, player);
            if (gameWorld.cooldownTimer > 0) gameWorld.cooldownTimer -= fTime;
        } else if (state[SDL_SCANCODE_SPACE]) gameWorld.reset(player);

        gameWorld.drawFrame(player);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    SDL_Quit(); return 0;
}