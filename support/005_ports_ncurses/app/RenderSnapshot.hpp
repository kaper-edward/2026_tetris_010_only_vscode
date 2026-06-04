#ifndef TETRIS_ASSIGNMENT_005_RENDER_SNAPSHOT_HPP
#define TETRIS_ASSIGNMENT_005_RENDER_SNAPSHOT_HPP

#include <array>
#include <cstdint>
#include <string>

struct LocalSnapshot;

namespace tetris::render {

constexpr int RENDER_BOARD_WIDTH = 10;
constexpr int RENDER_BOARD_HEIGHT = 20;
constexpr int RENDER_BLOCK_SIZE = 4;

struct PiecePreview {
    bool available = false;
    int tetromino = 0;
    std::array<std::string, RENDER_BLOCK_SIZE> rows{};
};

struct RenderSnapshot {
    std::uint32_t tick = 0;
    std::array<std::string, RENDER_BOARD_HEIGHT> board_rows{};
    bool has_color_board = false;
    std::array<std::string, RENDER_BOARD_HEIGHT> color_board_rows{};
    int current_tetromino = 0;
    int current_rotation = 0;
    int block_x = 0;
    int block_y = 0;
    bool has_ghost_y = false;
    int ghost_y = 0;
    PiecePreview next_piece;
    int score = 0;
    int level = 1;
    int lines_cleared = 0;
    bool running = true;
    std::string status_text;
};

RenderSnapshot to_render_snapshot(const ::LocalSnapshot& local);
std::string render_snapshot_to_text(const RenderSnapshot& snapshot);

}  // namespace tetris::render

#endif
