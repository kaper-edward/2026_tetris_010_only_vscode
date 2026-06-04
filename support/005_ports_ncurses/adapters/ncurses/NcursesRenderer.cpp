#include "NcursesRenderer.hpp"

#include <ncurses.h>

#include <cassert>
#include <cstddef>
#include <string>

using namespace tetris::render;

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

}  // namespace

void NcursesRenderer::initialize() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
}

void NcursesRenderer::shutdown() {
    endwin();
}

void NcursesRenderer::render(const RenderSnapshot& snapshot) {
    erase();
    const int offset_y = 1;
    const int offset_x = 2;
    for (int y = 0; y < RENDER_BOARD_HEIGHT; ++y) {
        mvaddch(offset_y + y, offset_x - 1, '|');
        for (int x = 0; x < RENDER_BOARD_WIDTH; ++x) {
            mvaddch(offset_y + y, offset_x + x, board_cell(snapshot, y, x));
        }
        mvaddch(offset_y + y, offset_x + RENDER_BOARD_WIDTH, '|');
    }
    mvprintw(offset_y + RENDER_BOARD_HEIGHT, offset_x - 1, "+----------+");

    const int side_x = offset_x + RENDER_BOARD_WIDTH + 4;
    mvprintw(offset_y, side_x, "Score: %d", snapshot.score);
    mvprintw(offset_y + 1, side_x, "Level: %d", snapshot.level);
    mvprintw(offset_y + 2, side_x, "Lines: %d", snapshot.lines_cleared);
    mvprintw(offset_y + 4, side_x, "Next:");
    if (snapshot.next_piece.available) {
        for (int y = 0; y < RENDER_BLOCK_SIZE; ++y) {
            for (int x = 0; x < RENDER_BLOCK_SIZE; ++x) {
                mvaddch(offset_y + 5 + y, side_x + x, preview_cell(snapshot.next_piece, y, x) == '#' ? '#' : ' ');
            }
        }
    }
    if (!snapshot.status_text.empty()) {
        mvprintw(offset_y + 10, side_x, "%s", snapshot.status_text.c_str());
    }
    if (!snapshot.running) {
        mvprintw(offset_y + 11, side_x, "Game Over");
    }
    refresh();
}
