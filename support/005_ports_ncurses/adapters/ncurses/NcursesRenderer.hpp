#ifndef TETRIS_ASSIGNMENT_005_NCURSES_RENDERER_HPP
#define TETRIS_ASSIGNMENT_005_NCURSES_RENDERER_HPP

#include "IRenderer.hpp"

class NcursesRenderer final : public IRenderer {
public:
    void initialize();
    void shutdown();
    void render(const tetris::render::RenderSnapshot& snapshot) override;
};

#endif
