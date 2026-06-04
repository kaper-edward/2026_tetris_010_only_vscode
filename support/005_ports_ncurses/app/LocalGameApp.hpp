#ifndef TETRIS_ASSIGNMENT_005_LOCAL_GAME_APP_HPP
#define TETRIS_ASSIGNMENT_005_LOCAL_GAME_APP_HPP

#include "IInput.hpp"
#include "IRenderer.hpp"
#include "tetris.h"

#include <chrono>

class LocalGameApp {
public:
    LocalGameApp(GameCore core, IRenderer& renderer, IInput& input, int tick_rate_hz = 20);

    LocalGameApp(const LocalGameApp&) = delete;
    LocalGameApp& operator=(const LocalGameApp&) = delete;
    LocalGameApp(LocalGameApp&&) = delete;
    LocalGameApp& operator=(LocalGameApp&&) = delete;

    void singleTick();
    void run(int max_ticks = -1);

private:
    GameCore core_;
    IRenderer& renderer_;  // non-owning; must outlive LocalGameApp
    IInput& input_;        // non-owning; must outlive LocalGameApp
    int tick_rate_hz_ = 20;
    int tick_count_ = 1;
};

#endif
