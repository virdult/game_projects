#ifndef GAME_HPP
#define GAME_HPP

#include <string>
#include <vector>
#include <memory>

namespace Block {
    
    class Block {
    public:
        Block();
        virtual ~Block() = default;
        virtual void draw();
        virtual std::vector<std::pair<int, int>> getShape() const;
        void setPosition(int x, int y);
        int getX() const;
        int getY() const;
        void moveLeft();
        void moveRight();
        void moveDown();
        virtual void rotate();
    
    protected:
        int x, y;
        int rotationState;
        std::vector<std::pair<int, int>> shape;
    };

    class BlockTypeI : public Block {
    public:
        BlockTypeI();
        void draw() override;
        std::vector<std::pair<int, int>> getShape() const override;
        void rotate() override;
    };

    class BlockTypeO : public Block {
    public:
        BlockTypeO();
        void draw() override;
        std::vector<std::pair<int, int>> getShape() const override;
    };

    class BlockTypeT : public Block {
    public:
        BlockTypeT();
        void draw() override;
        std::vector<std::pair<int, int>> getShape() const override;
        void rotate() override;
    };

    class BlockTypeS : public Block {
    public:
        BlockTypeS();
        void draw() override;
        std::vector<std::pair<int, int>> getShape() const override;
        void rotate() override;
    };

    class BlockTypeZ : public Block {
    public:
        BlockTypeZ();
        void draw() override;
        std::vector<std::pair<int, int>> getShape() const override;
        void rotate() override;
    };

    class BlockTypeJ : public Block {
    public:
        BlockTypeJ();
        void draw() override;
        std::vector<std::pair<int, int>> getShape() const override;
        void rotate() override;
    };

    class BlockTypeL : public Block {
    public:
        BlockTypeL();
        void draw() override;
        std::vector<std::pair<int, int>> getShape() const override;
        void rotate() override;
    };
} // namespace Block

namespace Game {

    const int GRID_WIDTH = 10;
    const int GRID_HEIGHT = 20;

    class Game {
    public:
        Game();
        ~Game();
        void start();
        void stop();
        void update();
        void render();
        void handleInput(char input);
        bool isGameRunning() const;
        bool isGameOver() const;
        std::string getStatus() const;
        int getScore() const;
    
    private:
        bool isRunning;
        bool gameOver;
        int score;
        int grid[GRID_HEIGHT][GRID_WIDTH];
        std::shared_ptr<Block::Block> currentBlock;
        std::shared_ptr<Block::Block> nextBlock;
        int fallCounter;
        
        void spawnBlock();
        bool checkCollision(int dx, int dy, const std::vector<std::pair<int, int>>& shape = {}) const;
        void lockBlock();
        void clearLines();
        void drawGrid() const;
        std::shared_ptr<Block::Block> createRandomBlock();
    };

} // namespace Game












#endif // GAME_HPP