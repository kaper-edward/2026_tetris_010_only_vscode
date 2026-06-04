#include "tetris.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ostream>
#include <sstream>
#include <stdexcept>

namespace {

using Rotations = std::array<Block, 4>;
using TetrominoCells = std::array<Position, 4>;

constexpr Block make_block(std::array<std::array<int, BLOCK_SIZE>, BLOCK_SIZE> rows) {
    return rows;
}

const std::array<Rotations, TETROMINO_COUNT>& tetrominoes() {
    static const std::array<Rotations, TETROMINO_COUNT> data{{
        Rotations{{
            make_block({{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}}}),
            make_block({{{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}}),
        }},
        Rotations{{
            make_block({{{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}),
        }},
        Rotations{{
            make_block({{{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}),
        }},
        Rotations{{
            make_block({{{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}),
        }},
        Rotations{{
            make_block({{{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}}}),
        }},
        Rotations{{
            make_block({{{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}}}),
        }},
        Rotations{{
            make_block({{{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}}),
            make_block({{{0, 0, 0, 0}, {1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}}}),
            make_block({{{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}),
        }},
    }};
    return data;
}

TetrominoCells tetromino_cells(Position origin, const Block& block) {
    TetrominoCells cells{};
    std::size_t index = 0;
    for (int y = 0; y < BLOCK_SIZE; ++y) {
        for (int x = 0; x < BLOCK_SIZE; ++x) {
            if (block[y][x] == 0) {
                continue;
            }
            if (index >= cells.size()) {
                throw std::logic_error("tetromino has more than 4 cells");
            }
            cells[index] = origin + Position{x, y};
            ++index;
        }
    }
    if (index != cells.size()) {
        throw std::logic_error("tetromino has fewer than 4 cells");
    }
    return cells;
}

std::size_t to_index(int value) {
    assert(value >= 0);
    return static_cast<std::size_t>(value);
}

bool is_on_board(Position position) {
    return position.x >= 0 && position.x < BOARD_WIDTH &&
           position.y >= 0 && position.y < BOARD_HEIGHT;
}

char board_cell(const Board& board, Position position) {
    return board[to_index(position.y)][to_index(position.x)];
}

char& board_cell(Board& board, Position position) {
    return board[to_index(position.y)][to_index(position.x)];
}

}  // namespace

Position operator+(Position lhs, Position rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

std::ostream& operator<<(std::ostream& out, Position position) {
    out << position.x << "," << position.y;
    return out;
}

std::ostream& operator<<(std::ostream& out, const LocalSnapshot& snapshot) {
    out << snapshot_to_text(snapshot);
    return out;
}

std::string snapshot_to_text(const LocalSnapshot& snapshot) {
    std::ostringstream out;
    out << "tick=" << snapshot.tick << "\n";
    out << "running=" << (snapshot.running ? 1 : 0) << "\n";
    out << "current=" << snapshot.current_tetromino << "\n";
    out << "rotation=" << snapshot.current_rotation << "\n";
    out << "position=" << snapshot.block_position << "\n";
    out << "next=" << snapshot.next_tetromino << "\n";
    out << "score=" << snapshot.score << "\n";
    out << "level=" << snapshot.level << "\n";
    out << "lines=" << snapshot.lines_cleared << "\n";
    for (const auto& row : snapshot.board_rows) {
        out << row << "\n";
    }
    return out.str();
}

GameCore::GameCore(std::uint32_t seed) : rng_(seed) {
    resetBoard();
    next_tetromino_ = 0;
    spawnNewBlock();
}

void GameCore::resetBoard() {
    for (auto& row : board_) {
        row.fill('.');
    }
    score_ = 0;
    level_ = 1;
    lines_cleared_ = 0;
    fall_interval_ = 10;
    tick_ = 0;
    running_ = true;
}

void GameCore::spawnNewBlock() {
    current_tetromino_ = next_tetromino_;
    current_rotation_ = 0;
    current_block_ = tetromino_block(current_tetromino_, current_rotation_);
    block_position_ = {BOARD_WIDTH / 2 - 2, 0};
    next_tetromino_ = static_cast<int>(rng_() % TETROMINO_COUNT);
    if (!isValidPosition(block_position_, current_block_)) {
        running_ = false;
    }
}

bool GameCore::isValidPosition(Position position, const Block& block) const {
    for (Position board_position : tetromino_cells(position, block)) {
        if (!is_on_board(board_position)) {
            return false;
        }
        if (board_cell(board_, board_position) != '.') {
            return false;
        }
    }
    return true;
}

bool GameCore::moveBlock(int dx, int dy) {
    const Position next = block_position_ + Position{dx, dy};
    if (!isValidPosition(next, current_block_)) {
        return false;
    }
    block_position_ = next;
    return true;
}

void GameCore::rotateBlock() {
    const int next_rotation = (current_rotation_ + 1) % tetromino_rotation_count(current_tetromino_);
    const Block& rotated = tetromino_block(current_tetromino_, next_rotation);
    if (isValidPosition(block_position_, rotated)) {
        current_rotation_ = next_rotation;
        current_block_ = rotated;
    }
}

void GameCore::hardDrop() {
    while (moveBlock(0, 1)) {
    }
    lockBlock();
    spawnNewBlock();
}

void GameCore::lockBlock() {
    for (Position position : tetromino_cells(block_position_, current_block_)) {
        board_cell(board_, position) = '#';
    }
    checkLines();
}

void GameCore::checkLines() {
    int cleared = 0;
    for (int y = BOARD_HEIGHT - 1; y >= 0; --y) {
        const bool full = std::ranges::all_of(board_[y], [](char cell) {
            return cell == '#';
        });
        if (!full) {
            continue;
        }
        for (int move_y = y; move_y > 0; --move_y) {
            board_[move_y] = board_[move_y - 1];
        }
        board_[0].fill('.');
        ++cleared;
        ++y;
    }
    if (cleared > 0) {
        lines_cleared_ += cleared;
        score_ += cleared * 100 * level_;
        level_ = lines_cleared_ / 10 + 1;
        fall_interval_ = std::max(1, 11 - level_);
    }
}

void GameCore::applyInput(GameAction action) {
    if (!running_ && action != GameAction::Quit) {
        return;
    }
    switch (action) {
        case GameAction::Left:
            moveBlock(-1, 0);
            break;
        case GameAction::Right:
            moveBlock(1, 0);
            break;
        case GameAction::Down:
            if (!moveBlock(0, 1)) {
                lockBlock();
                spawnNewBlock();
            }
            break;
        case GameAction::Rotate:
            rotateBlock();
            break;
        case GameAction::Drop:
            hardDrop();
            break;
        case GameAction::Quit:
            running_ = false;
            break;
        case GameAction::None:
            break;
    }
}

void GameCore::step() {
    if (!running_) {
        return;
    }
    ++tick_;
    if (tick_ % static_cast<std::uint32_t>(fall_interval_) == 0 && !moveBlock(0, 1)) {
        lockBlock();
        spawnNewBlock();
    }
}

LocalSnapshot GameCore::snapshot() const {
    LocalSnapshot snapshot;
    snapshot.tick = tick_;
    snapshot.current_tetromino = current_tetromino_;
    snapshot.current_rotation = current_rotation_;
    snapshot.block_position = block_position_;
    snapshot.next_tetromino = next_tetromino_;
    snapshot.score = score_;
    snapshot.level = level_;
    snapshot.lines_cleared = lines_cleared_;
    snapshot.running = running_;

    Board rendered = board_;
    for (Position position : tetromino_cells(block_position_, current_block_)) {
        if (is_on_board(position)) {
            board_cell(rendered, position) = '#';
        }
    }
    std::ranges::transform(rendered, snapshot.board_rows.begin(), [](const auto& row) {
        return std::string(row.begin(), row.end());
    });
    return snapshot;
}

void GameCore::setBoardCellForTesting(Position position, char value) {
    if (is_on_board(position)) {
        board_cell(board_, position) = value;
    }
}

int GameCore::fallIntervalForTesting() const {
    return fall_interval_;
}

GameAction parse_action(const std::string& token) {
    if (token == "LEFT") return GameAction::Left;
    if (token == "RIGHT") return GameAction::Right;
    if (token == "DOWN") return GameAction::Down;
    if (token == "ROTATE") return GameAction::Rotate;
    if (token == "DROP") return GameAction::Drop;
    if (token == "QUIT") return GameAction::Quit;
    return GameAction::None;
}

std::string action_to_string(GameAction action) {
    switch (action) {
        case GameAction::Left: return "LEFT";
        case GameAction::Right: return "RIGHT";
        case GameAction::Down: return "DOWN";
        case GameAction::Rotate: return "ROTATE";
        case GameAction::Drop: return "DROP";
        case GameAction::Quit: return "QUIT";
        case GameAction::None: return "NONE";
    }
    return "NONE";
}

const Block& tetromino_block(int tetromino, int rotation) {
    if (tetromino < 0 || tetromino >= TETROMINO_COUNT) {
        throw std::out_of_range("tetromino");
    }
    return tetrominoes()[tetromino][rotation % 4];
}

int tetromino_rotation_count(int tetromino) {
    if (tetromino == 1) {
        return 1;
    }
    if (tetromino == 0) {
        return 2;
    }
    return 4;
}
