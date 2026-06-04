#ifndef TETRIS_ASSIGNMENT_004_TETRIS_H
#define TETRIS_ASSIGNMENT_004_TETRIS_H

#include <array>
#include <cstdint>
#include <iosfwd>
#include <random>
#include <string>

#include "../shared/core/GameAction.hpp"

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;
constexpr int BLOCK_SIZE = 4;
constexpr int TETROMINO_COUNT = 7;

struct Position {
    int x = 0;
    int y = 0;

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

Position operator+(Position lhs, Position rhs);
std::ostream& operator<<(std::ostream& out, Position position);

using Board = std::array<std::array<char, BOARD_WIDTH>, BOARD_HEIGHT>;
using Block = std::array<std::array<int, BLOCK_SIZE>, BLOCK_SIZE>;

struct LocalSnapshot {
    std::uint32_t tick = 0;
    std::array<std::string, BOARD_HEIGHT> board_rows{};
    int current_tetromino = 0;
    int current_rotation = 0;
    Position block_position{};
    int next_tetromino = 0;
    int score = 0;
    int level = 1;
    int lines_cleared = 0;
    bool running = true;
};

std::ostream& operator<<(std::ostream& out, const LocalSnapshot& snapshot);
std::string snapshot_to_text(const LocalSnapshot& snapshot);

class GameCore {
public:
    explicit GameCore(std::uint32_t seed = 888);

    void applyInput(GameAction action);
    void step();
    LocalSnapshot snapshot() const;

    void setBoardCellForTesting(Position position, char value);
    int fallIntervalForTesting() const;

private:
    Board board_{};
    Block current_block_{};
    int current_tetromino_ = 0;
    int current_rotation_ = 0;
    Position block_position_{};
    int next_tetromino_ = 0;
    int score_ = 0;
    int level_ = 1;
    int lines_cleared_ = 0;
    int fall_interval_ = 10;
    std::uint32_t tick_ = 0;
    bool running_ = true;
    std::mt19937 rng_;

    void resetBoard();
    void spawnNewBlock();
    bool isValidPosition(Position position, const Block& block) const;
    bool moveBlock(int dx, int dy);
    void rotateBlock();
    void hardDrop();
    void lockBlock();
    void checkLines();
};

GameAction parse_action(const std::string& token);
std::string action_to_string(GameAction action);
const Block& tetromino_block(int tetromino, int rotation);
int tetromino_rotation_count(int tetromino);

#endif
