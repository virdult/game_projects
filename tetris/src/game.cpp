#include <iostream>
#include <cstdlib>
#include <ctime>
#include "game.hpp"

namespace Block {

    // Base Block class
    Block::Block() : x(0), y(0), rotationState(0) {}

    void Block::draw() {
        std::cout << "Drawing a generic block." << std::endl;
    }

    std::vector<std::pair<int, int>> Block::getShape() const {
        return shape;
    }

    void Block::setPosition(int nx, int ny) {
        x = nx;
        y = ny;
    }

    int Block::getX() const { return x; }
    int Block::getY() const { return y; }

    void Block::moveLeft() {
        x--;
    }

    void Block::moveRight() {
        x++;
    }

    void Block::moveDown() {
        y++;
    }

    void Block::rotate() {
        // Default no rotation for base class
    }

    // BlockTypeI: Vertical line
    BlockTypeI::BlockTypeI() : Block() {
        shape = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};
    }

    void BlockTypeI::draw() {
        for (int i = 0; i < 4; ++i) {
            std::cout << "|" << std::endl;
        }
    }

    std::vector<std::pair<int, int>> BlockTypeI::getShape() const {
        return shape;
    }

    void BlockTypeI::rotate() {
        rotationState = (rotationState + 1) % 2;
        if (rotationState == 0) {
            shape = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};
        } else {
            shape = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};
        }
    }

    // BlockTypeO: Square
    BlockTypeO::BlockTypeO() : Block() {
        shape = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    }

    void BlockTypeO::draw() {
        std::cout << "##" << std::endl;
        std::cout << "##" << std::endl;
    }

    std::vector<std::pair<int, int>> BlockTypeO::getShape() const {
        return shape;
    }

    // BlockTypeT: T shape
    BlockTypeT::BlockTypeT() : Block() {
        shape = {{0, 0}, {1, 0}, {2, 0}, {1, 1}};
    }

    void BlockTypeT::draw() {
        std::cout << "###" << std::endl;
        std::cout << " # " << std::endl;
    }

    std::vector<std::pair<int, int>> BlockTypeT::getShape() const {
        return shape;
    }

    void BlockTypeT::rotate() {
        // Rotate T piece through 4 states
        rotationState = (rotationState + 1) % 4;
        switch (rotationState) {
            case 0:
                shape = {{0, 0}, {1, 0}, {2, 0}, {1, 1}};
                break;
            case 1:
                shape = {{0, 0}, {0, 1}, {0, 2}, {1, 1}};
                break;
            case 2:
                shape = {{0, 1}, {1, 1}, {2, 1}, {1, 0}};
                break;
            case 3:
                shape = {{1, 0}, {1, 1}, {1, 2}, {0, 1}};
                break;
        }
    }

    // BlockTypeS: S shape
    BlockTypeS::BlockTypeS() : Block() {
        shape = {{1, 0}, {2, 0}, {0, 1}, {1, 1}};
    }

    void BlockTypeS::draw() {
        std::cout << " ##" << std::endl;
        std::cout << "## " << std::endl;
    }

    std::vector<std::pair<int, int>> BlockTypeS::getShape() const {
        return shape;
    }

    void BlockTypeS::rotate() {
        rotationState = (rotationState + 1) % 2;
        if (rotationState == 0) {
            shape = {{1, 0}, {2, 0}, {0, 1}, {1, 1}};
        } else {
            shape = {{0, 0}, {0, 1}, {1, 1}, {1, 2}};
        }
    }

    // BlockTypeZ: Z shape
    BlockTypeZ::BlockTypeZ() : Block() {
        shape = {{0, 0}, {1, 0}, {1, 1}, {2, 1}};
    }

    void BlockTypeZ::draw() {
        std::cout << "## " << std::endl;
        std::cout << " ##" << std::endl;
    }

    std::vector<std::pair<int, int>> BlockTypeZ::getShape() const {
        return shape;
    }

    void BlockTypeZ::rotate() {
        rotationState = (rotationState + 1) % 2;
        if (rotationState == 0) {
            shape = {{0, 0}, {1, 0}, {1, 1}, {2, 1}};
        } else {
            shape = {{1, 0}, {0, 1}, {1, 1}, {0, 2}};
        }
    }

    // BlockTypeJ: J shape
    BlockTypeJ::BlockTypeJ() : Block() {
        shape = {{0, 0}, {0, 1}, {1, 1}, {2, 1}};
    }

    void BlockTypeJ::draw() {
        std::cout << "#  " << std::endl;
        std::cout << "###" << std::endl;
    }

    std::vector<std::pair<int, int>> BlockTypeJ::getShape() const {
        return shape;
    }

    void BlockTypeJ::rotate() {
        rotationState = (rotationState + 1) % 4;
        switch (rotationState) {
            case 0:
                shape = {{1, 0}, {1, 1}, {0, 2}, {1, 2}};
                break;
            case 1:
                shape = {{0, 0}, {1, 0}, {2, 0}, {2, 1}};
                break;
            case 2:
                shape = {{0, 0}, {1, 0}, {0, 1}, {0, 2}};
                break;
            case 3:
                shape = {{0, 0}, {0, 1}, {1, 1}, {2, 1}};
                break;
        }
    }

    // BlockTypeL: L shape
    BlockTypeL::BlockTypeL() : Block() {
        shape = {{2, 0}, {0, 1}, {1, 1}, {2, 1}};
    }

    void BlockTypeL::draw() {
        std::cout << "  #" << std::endl;
        std::cout << "###" << std::endl;
    }

    std::vector<std::pair<int, int>> BlockTypeL::getShape() const {
        return shape;
    }

    void BlockTypeL::rotate() {
        rotationState = (rotationState + 1) % 4;
        switch (rotationState) {
            case 0:
                shape = {{0, 0}, {0, 1}, {0, 2}, {1, 2}};
                break;
            case 1:
                shape = {{0, 0}, {1, 0}, {2, 0}, {0, 1}};
                break;
            case 2:
                shape = {{0, 0}, {1, 0}, {1, 1}, {1, 2}};
                break;
            case 3:
                shape = {{2, 0}, {0, 1}, {1, 1}, {2, 1}};
                break;
        }
    }

} // namespace Block

namespace Game {

    Game::Game() : isRunning(false), gameOver(false), score(0), fallCounter(0) {
        // Initialize grid with 0s (empty cells)
        for (int i = 0; i < GRID_HEIGHT; ++i) {
            for (int j = 0; j < GRID_WIDTH; ++j) {
                grid[i][j] = 0;
            }
        }
        srand(time(0));
    }

    Game::~Game() {}

    void Game::start() {
        isRunning = true;
        gameOver = false;
        score = 0;
        fallCounter = 0;
        
        // Initialize grid
        for (int i = 0; i < GRID_HEIGHT; ++i) {
            for (int j = 0; j < GRID_WIDTH; ++j) {
                grid[i][j] = 0;
            }
        }
        
        spawnBlock();
        nextBlock = createRandomBlock();
    }

    void Game::stop() {
        isRunning = false;
    }

    bool Game::isGameRunning() const {
        return isRunning;
    }

    bool Game::isGameOver() const {
        return gameOver;
    }

    std::string Game::getStatus() const {
        if (gameOver) return "Game Over";
        return isRunning ? "Running" : "Stopped";
    }

    int Game::getScore() const {
        return score;
    }

    std::shared_ptr<Block::Block> Game::createRandomBlock() {
        int type = rand() % 7;
        switch (type) {
            case 0: return std::make_shared<Block::BlockTypeI>();
            case 1: return std::make_shared<Block::BlockTypeO>();
            case 2: return std::make_shared<Block::BlockTypeT>();
            case 3: return std::make_shared<Block::BlockTypeS>();
            case 4: return std::make_shared<Block::BlockTypeZ>();
            case 5: return std::make_shared<Block::BlockTypeJ>();
            case 6: return std::make_shared<Block::BlockTypeL>();
            default: return std::make_shared<Block::BlockTypeI>();
        }
    }

    void Game::spawnBlock() {
        currentBlock = nextBlock;
        if (!currentBlock) {
            currentBlock = createRandomBlock();
        }
        nextBlock = createRandomBlock(); // Create fresh unrotated next block
        currentBlock->setPosition(GRID_WIDTH / 2 - 1, 0);
        
        // Check if spawn position is blocked (game over)
        if (checkCollision(0, 0, currentBlock->getShape())) {
            gameOver = true;
            isRunning = false;
        }
    }

    bool Game::checkCollision(int dx, int dy, const std::vector<std::pair<int, int>>& shapeOverride) const {
        if (!currentBlock) return false;
        
        auto shape = shapeOverride.empty() ? currentBlock->getShape() : shapeOverride;
        int newX = currentBlock->getX() + dx;
        int newY = currentBlock->getY() + dy;
        
        for (const auto& cell : shape) {
            int cellX = newX + cell.first;
            int cellY = newY + cell.second;
            
            // Check boundaries
            if (cellX < 0 || cellX >= GRID_WIDTH || cellY >= GRID_HEIGHT) {
                return true;
            }
            
            // Check if cell is already occupied (ignore cells above grid)
            if (cellY >= 0 && grid[cellY][cellX] != 0) {
                return true;
            }
        }
        return false;
    }

    void Game::lockBlock() {
        if (!currentBlock) return;
        
        auto shape = currentBlock->getShape();
        int blockX = currentBlock->getX();
        int blockY = currentBlock->getY();
        
        for (const auto& cell : shape) {
            int cellX = blockX + cell.first;
            int cellY = blockY + cell.second;
            
            if (cellY >= 0 && cellY < GRID_HEIGHT && cellX >= 0 && cellX < GRID_WIDTH) {
                grid[cellY][cellX] = 1;
            }
        }
        
        clearLines();
        spawnBlock();
    }

    void Game::clearLines() {
        int linesCleared = 0;
        
        for (int i = GRID_HEIGHT - 1; i >= 0; --i) {
            bool fullLine = true;
            for (int j = 0; j < GRID_WIDTH; ++j) {
                if (grid[i][j] == 0) {
                    fullLine = false;
                    break;
                }
            }
            
            if (fullLine) {
                linesCleared++;
                // Shift rows down
                for (int k = i; k > 0; --k) {
                    for (int j = 0; j < GRID_WIDTH; ++j) {
                        grid[k][j] = grid[k-1][j];
                    }
                }
                // Clear top row
                for (int j = 0; j < GRID_WIDTH; ++j) {
                    grid[0][j] = 0;
                }
                i++; // Recheck this row
            }
        }
        
        // Award more points for multiple lines cleared at once
        if (linesCleared > 0) {
            switch (linesCleared) {
                case 1: score += 100; break;
                case 2: score += 300; break;
                case 3: score += 500; break;
                case 4: score += 800; break;
                default: score += linesCleared * 200; break;
            }
        }
    }

    void Game::update() {
        if (!isRunning || gameOver) return;
        
        // Calculate speed based on score
        int level = std::min(29, score / 100);
        int baseSpeed = 48 - (level * 5 / 3);
        int threshold = baseSpeed / 5;
        int speedThreshold = std::max(1, (threshold * 4) / 5);
        
        fallCounter++;
        if (fallCounter >= speedThreshold) {
            fallCounter = 0;
            
            if (!checkCollision(0, 1)) {
                currentBlock->moveDown();
            } else {
                lockBlock();
            }
        }
    }

    void Game::handleInput(char input) {
        if (!isRunning || gameOver) return;
        
        switch (input) {
            case 'a': // Move left
            case 'A':
                if (!checkCollision(-1, 0)) {
                    currentBlock->moveLeft();
                }
                break;
            case 'd': // Move right
            case 'D':
                if (!checkCollision(1, 0)) {
                    currentBlock->moveRight();
                }
                break;
            case 's': // Move down
            case 'S':
                if (!checkCollision(0, 1)) {
                    currentBlock->moveDown();
                }
                break;
            case 'w': // Rotate
            case 'W':
                currentBlock->rotate();
                if (checkCollision(0, 0)) {
                    currentBlock->rotate(); // Undo rotation if collision
                }
                break;
            case 'q': // Quit
            case 'Q':
                stop();
                break;
        }
    }

    void Game::drawGrid() const {
        // Clear screen (works on Linux/Mac)
        std::cout << "\033[2J\033[1;1H";
        
        std::cout << "=== TETRIS ===" << std::endl;
        
        // Top border
        std::cout << "+-";
        for (int i = 1; i < GRID_WIDTH * 2; ++i) std::cout << "-";
        std::cout << "-+" << std::endl;
        
        // Grid with current block
        for (int i = 0; i < GRID_HEIGHT; ++i) {
            std::cout << "| ";
            for (int j = 0; j < GRID_WIDTH; ++j) {
                if (currentBlock) {
                    auto shape = currentBlock->getShape();
                    int blockX = currentBlock->getX();
                    int blockY = currentBlock->getY();
                    
                    bool isCurrent = false;
                    for (const auto& cell : shape) {
                        if (blockX + cell.first == j && blockY + cell.second == i) {
                            isCurrent = true;
                            break;
                        }
                    }
                    
                    if (isCurrent) {
                        std::cout << "# ";
                    } else if (grid[i][j] != 0) {
                        std::cout << "# ";
                    } else {
                        std::cout << ". ";
                    }
                } else if (grid[i][j] != 0) {
                    std::cout << "# ";
                } else {
                    std::cout << ". ";
                }
            }
            std::cout << "|";
            
            // Display score and next block on the right
            if (i == 0) {
                std::cout << "    Score: " << score;
            } else if (i == 2) {
                std::cout << "    Next:";
            } else if (i >= 3 && i <= 6 && nextBlock) {
                auto nextShape = nextBlock->getShape();
                std::cout << "    ";
                for (int col = 0; col < 3; ++col) {
                    bool hasBlock = false;
                    for (const auto& cell : nextShape) {
                        if (cell.first == col && cell.second == (i - 3)) {
                            hasBlock = true;
                            break;
                        }
                    }
                    std::cout << (hasBlock ? "#" : " ");
                }
            }
            
            std::cout << std::endl;
        }
        
        // Bottom border
        std::cout << "+-";
        for (int i = 1; i < GRID_WIDTH * 2; ++i) std::cout << "-";
        std::cout << "-+" << std::endl;
        
        std::cout << "Controls: A/D (move), S (down), W (rotate), Q (quit)" << std::endl;
        std::cout << "Status: " << getStatus() << std::endl;
    }

    void Game::render() {
        drawGrid();
    }

} // namespace Game