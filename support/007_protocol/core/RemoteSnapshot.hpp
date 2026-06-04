#ifndef TETRIS_ASSIGNMENT_007_REMOTE_SNAPSHOT_HPP
#define TETRIS_ASSIGNMENT_007_REMOTE_SNAPSHOT_HPP

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace tetris::protocol {

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;
using MatchId = int;
using PlayerId = int;

struct ScoreEntry {
    std::string name;
    int score = 0;
};

struct RemoteSnapshot {
    std::uint32_t tick = 0;
    bool running = true;
    int current_tetromino = 0;
    int current_rotation = 0;
    int block_x = 0;
    int block_y = 0;
    bool has_ghost_y = false;
    int ghost_y = 0;
    int score = 0;
    int level = 1;
    int lines_cleared = 0;
    std::array<std::string, BOARD_HEIGHT> board_rows{};
    bool has_color_board = false;
    std::array<std::string, BOARD_HEIGHT> color_board_rows{};
};

struct RemotePlayerSnapshot {
    PlayerId player = 0;
    std::string name;
    bool running = true;
    // Parsed for protocol compatibility. Versus renderers use board_rows as
    // authoritative because the server already includes the active piece.
    int current_tetromino = 0;
    int current_rotation = 0;
    int block_x = 0;
    int block_y = 0;
    bool has_ghost_y = false;
    int ghost_y = 0;
    bool has_next = false;
    int next_tetromino = 0;
    int score = 0;
    int level = 1;
    int lines_cleared = 0;
    std::array<std::string, BOARD_HEIGHT> board_rows{};
    bool has_color_board = false;
    std::array<std::string, BOARD_HEIGHT> color_board_rows{};
};

struct RemoteMatchSnapshot {
    MatchId match = 0;
    std::uint32_t tick = 0;
    PlayerId you = 0;
    PlayerId opponent = 0;
    bool has_high_score = false;
    int high_score = 0;
    std::vector<ScoreEntry> top_scores;
    // players[0] is player id 1 (p1_*), players[1] is player id 2 (p2_*).
    std::array<RemotePlayerSnapshot, 2> players{};
};

}  // namespace tetris::protocol

#endif
