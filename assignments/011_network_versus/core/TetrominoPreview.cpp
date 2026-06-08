#include "TetrominoPreview.hpp"

namespace tetris::render {

std::array<std::string, 4> tetromino_preview_rows(int /*tetromino*/) {
    return {"....", "....", "....", "...."};
}

std::vector<std::pair<int, int>> tetromino_cells(int /*tetromino*/, int /*rotation*/) {
    return {};
}

}  // namespace tetris::render
