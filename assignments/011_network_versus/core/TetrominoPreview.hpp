#ifndef TETRIS_ASSIGNMENT_011_TETROMINO_PREVIEW_HPP
#define TETRIS_ASSIGNMENT_011_TETROMINO_PREVIEW_HPP

#include <array>
#include <utility>
#include <string>
#include <vector>

namespace tetris::render {

std::array<std::string, 4> tetromino_preview_rows(int tetromino);
std::vector<std::pair<int, int>> tetromino_cells(int tetromino, int rotation);

}  // namespace tetris::render

#endif
