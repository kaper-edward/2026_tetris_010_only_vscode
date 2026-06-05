#include "NcursesVersusRenderer.hpp"

#include <ncurses.h>

#include <cassert>
#include <cstddef>
#include <string>

using namespace tetris::render;
using namespace tetris::view;

namespace {

std::size_t to_index(int value) {
    assert(value >= 0);
    return static_cast<std::size_t>(value);
}

char board_cell(const RenderSnapshot& snapshot, int row, int col) {
    if (row < 0 || row >= RENDER_BOARD_HEIGHT || col < 0 || col >= RENDER_BOARD_WIDTH) {
        return '.';
    }
    const std::string& cells = snapshot.board_rows[to_index(row)];
    if (static_cast<int>(cells.size()) <= col) {
        return '.';
    }
    return cells[to_index(col)];
}

char preview_cell(const PiecePreview& preview, int row, int col) {
    if (!preview.available || row < 0 || row >= RENDER_BLOCK_SIZE || col < 0 || col >= RENDER_BLOCK_SIZE) {
        return '.';
    }
    const std::string& cells = preview.rows[to_index(row)];
    if (static_cast<int>(cells.size()) <= col) {
        return '.';
    }
    return cells[to_index(col)];
}

void draw_player(const PlayerViewSnapshot& player, int origin_y, int origin_x) {
    mvprintw(origin_y, origin_x, "%s %s P%d", player.is_you ? "YOU" : "OPP", player.name.c_str(), player.player);
    mvprintw(origin_y + 1, origin_x, "score=%d level=%d lines=%d", player.board.score, player.board.level, player.board.lines_cleared);
    mvprintw(origin_y + 2, origin_x, "next");
    for (int y = 0; y < RENDER_BLOCK_SIZE; ++y) {
        for (int x = 0; x < RENDER_BLOCK_SIZE; ++x) {
            mvaddch(origin_y + 3 + y, origin_x + x, preview_cell(player.board.next_piece, y, x) == '#' ? '#' : ' ');
        }
    }
    const int board_y = origin_y + 8;
    for (int y = 0; y < RENDER_BOARD_HEIGHT; ++y) {
        mvaddch(board_y + y, origin_x, '|');
        for (int x = 0; x < RENDER_BOARD_WIDTH; ++x) {
            mvaddch(board_y + y, origin_x + 1 + x, board_cell(player.board, y, x) == '#' ? '#' : ' ');
        }
        mvaddch(board_y + y, origin_x + RENDER_BOARD_WIDTH + 1, '|');
    }
}

}  // namespace

void NcursesVersusRenderer::render(const VersusViewSnapshot& snapshot) {
    erase();
    mvprintw(0, 0, "status=%s tick=%u winner=%d %s",
             snapshot.status_text.empty() ? versus_phase_name(snapshot.phase) : snapshot.status_text.c_str(),
             snapshot.tick,
             snapshot.winner,
             snapshot.reason.c_str());
    draw_player(snapshot.you, 2, 0);
    draw_player(snapshot.opponent, 2, 32);
    refresh();
}
