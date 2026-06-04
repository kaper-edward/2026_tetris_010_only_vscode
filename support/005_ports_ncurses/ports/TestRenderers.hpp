#ifndef TETRIS_ASSIGNMENT_005_TEST_RENDERERS_HPP
#define TETRIS_ASSIGNMENT_005_TEST_RENDERERS_HPP

#include "IRenderer.hpp"

#include <vector>

class NullRenderer final : public IRenderer {
public:
    void render(const tetris::render::RenderSnapshot&) override {}
};

class CaptureRenderer final : public IRenderer {
public:
    void render(const tetris::render::RenderSnapshot& snapshot) override {
        snapshots_.push_back(snapshot);
    }

    const std::vector<tetris::render::RenderSnapshot>& snapshots() const {
        return snapshots_;
    }

private:
    std::vector<tetris::render::RenderSnapshot> snapshots_;
};

#endif
