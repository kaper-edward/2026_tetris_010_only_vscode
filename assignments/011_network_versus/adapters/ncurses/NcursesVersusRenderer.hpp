#ifndef TETRIS_ASSIGNMENT_011_NCURSES_VERSUS_RENDERER_HPP
#define TETRIS_ASSIGNMENT_011_NCURSES_VERSUS_RENDERER_HPP

#include "VersusViewSnapshot.hpp"

class NcursesVersusRenderer {
public:
    void render(const tetris::view::VersusViewSnapshot& snapshot);
};

#endif
