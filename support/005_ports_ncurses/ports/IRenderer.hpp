#ifndef TETRIS_ASSIGNMENT_005_IRENDERER_HPP
#define TETRIS_ASSIGNMENT_005_IRENDERER_HPP

#include "RenderSnapshot.hpp"

class IRenderer {
public:
    IRenderer() = default;
    IRenderer(const IRenderer&) = delete;
    IRenderer& operator=(const IRenderer&) = delete;
    virtual ~IRenderer() = default;

    virtual void render(const tetris::render::RenderSnapshot& snapshot) = 0;
};

#endif
