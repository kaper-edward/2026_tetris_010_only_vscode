#include "RenderSnapshot.hpp"

#include "tetris.h"

#include <sstream>

namespace tetris::render {

RenderSnapshot to_render_snapshot(const ::LocalSnapshot& local) {
    RenderSnapshot render;
    render.tick = local.tick;
    render.board_rows = local.board_rows;
    render.next_piece.available = true;
    render.next_piece.tetromino = local.next_tetromino;
    render.score = local.score;
    render.level = local.level;
    render.lines_cleared = local.lines_cleared;
    render.running = local.running;

    const Block& preview = tetromino_block(local.next_tetromino, 0);
    for (int y = 0; y < RENDER_BLOCK_SIZE; ++y) {
        std::string row;
        row.reserve(RENDER_BLOCK_SIZE);
        for (int x = 0; x < RENDER_BLOCK_SIZE; ++x) {
            row.push_back(preview[y][x] ? '#' : '.');
        }
        render.next_piece.rows[y] = row;
    }
    return render;
}

std::string render_snapshot_to_text(const RenderSnapshot& snapshot) {
    std::ostringstream out;
    out << "tick=" << snapshot.tick << "\n";
    out << "running=" << (snapshot.running ? 1 : 0) << "\n";
    out << "score=" << snapshot.score << "\n";
    out << "level=" << snapshot.level << "\n";
    out << "lines=" << snapshot.lines_cleared << "\n";
    if (!snapshot.status_text.empty()) {
        out << "status=" << snapshot.status_text << "\n";
    }
    out << "next_available=" << (snapshot.next_piece.available ? 1 : 0) << "\n";
    out << "next=" << snapshot.next_piece.tetromino << "\n";
    out << "board\n";
    for (const auto& row : snapshot.board_rows) {
        out << row << "\n";
    }
    out << "next_preview\n";
    for (const auto& row : snapshot.next_piece.rows) {
        out << row << "\n";
    }
    return out.str();
}

}  // namespace tetris::render
