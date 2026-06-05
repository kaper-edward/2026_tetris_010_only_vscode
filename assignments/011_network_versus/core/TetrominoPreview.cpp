#include "TetrominoPreview.hpp"

namespace tetris::render {

namespace {

using Rows = std::array<std::string, 4>;

const std::array<std::vector<Rows>, 7> SHAPES{{
    std::vector<Rows>{{"....", "####", "....", "...."}, {"..#.", "..#.", "..#.", "..#."}},
    std::vector<Rows>{{".##.", ".##.", "....", "...."}},
    std::vector<Rows>{{".#..", "###.", "....", "...."}, {".#..", ".##.", ".#..", "...."}, {"....", "###.", ".#..", "...."}, {".#..", "##..", ".#..", "...."}},
    std::vector<Rows>{{".##.", "##..", "....", "...."}, {".#..", ".##.", "..#.", "...."}, {"....", ".##.", "##..", "...."}, {"#...", "##..", ".#..", "...."}},
    std::vector<Rows>{{"##..", ".##.", "....", "...."}, {"..#.", ".##.", ".#..", "...."}, {"....", "##..", ".##.", "...."}, {".#..", "##..", "#...", "...."}},
    std::vector<Rows>{{"#...", "###.", "....", "...."}, {".##.", ".#..", ".#..", "...."}, {"....", "###.", "..#.", "...."}, {".#..", ".#..", "##..", "...."}},
    std::vector<Rows>{{"..#.", "###.", "....", "...."}, {".#..", ".#..", ".##.", "...."}, {"....", "###.", "#...", "...."}, {"##..", ".#..", ".#..", "...."}},
}};

Rows shape_rows(int tetromino, int rotation) {
    if (tetromino < 0 || tetromino >= static_cast<int>(SHAPES.size())) {
        return {"....", "....", "....", "...."};
    }
    const auto& rotations = SHAPES[static_cast<std::size_t>(tetromino)];
    const int index = rotation < 0 ? 0 : rotation % static_cast<int>(rotations.size());
    return rotations[static_cast<std::size_t>(index)];
}

}  // namespace

std::array<std::string, 4> tetromino_preview_rows(int tetromino) {
    return shape_rows(tetromino, 0);
}

std::vector<std::pair<int, int>> tetromino_cells(int tetromino, int rotation) {
    const Rows rows = shape_rows(tetromino, rotation);
    std::vector<std::pair<int, int>> cells;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (rows[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] == '#') {
                cells.push_back({col, row});
            }
        }
    }
    return cells;
}

}  // namespace tetris::render
