#include <iostream>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include "game.hpp"

// Terminal input handling for non-blocking input
class TerminalInput {
private:
    struct termios oldSettings, newSettings;
    
public:
    TerminalInput() {
        tcgetattr(STDIN_FILENO, &oldSettings);
        newSettings = oldSettings;
        newSettings.c_lflag &= ~(ICANON | ECHO);
        newSettings.c_cc[VMIN] = 0;
        newSettings.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
    }
    
    ~TerminalInput() {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
    }
    
    bool hasInput() const {
        return true; // Always check for input
    }
    
    char getInput() {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) > 0) {
            // Clear any buffered input to prevent key hold issues
            tcflush(STDIN_FILENO, TCIFLUSH);
            return c;
        }
        return '\0';
    }
};

int main() {
    Game::Game game;
    TerminalInput input;
    
    std::cout << "\033[?25l"; // Hide cursor
    std::cout << "Welcome to Terminal Tetris!" << std::endl;
    std::cout << "Starting in 2 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    game.start();
    
    // Game loop
    while (game.isGameRunning()) {
        // Handle input
        char c = input.getInput();
        if (c != '\0') {
            game.handleInput(c);
        }
        
        // Update game state
        game.update();
        
        // Render
        game.render();
        
        // Sleep for a short time to control frame rate
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Game over screen
    game.render();
    std::cout << std::endl;
    std::cout << "GAME OVER!" << std::endl;
    std::cout << "Final Score: " << game.getScore() << std::endl;
    
    std::cout << "\033[?25h"; // Show cursor
    
    return 0;
}
