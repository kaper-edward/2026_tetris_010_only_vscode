#include "LocalGameApp.hpp"

#include "RenderSnapshot.hpp"

#include <thread>

LocalGameApp::LocalGameApp(GameCore core, IRenderer& renderer, IInput& input, int tick_rate_hz)
    : core_(std::move(core)), renderer_(renderer), input_(input), tick_rate_hz_(tick_rate_hz) {}

void LocalGameApp::singleTick() {
    input_.advanceTick(tick_count_);
    if (auto action = input_.pollAction()) {
        core_.applyInput(*action);
    }
    core_.step();
    renderer_.render(tetris::render::to_render_snapshot(core_.snapshot()));
    ++tick_count_;
}

void LocalGameApp::run(int max_ticks) {
    const auto frame = std::chrono::milliseconds(1000 / tick_rate_hz_);
    int count = 0;
    while (max_ticks < 0 || count < max_ticks) {
        const auto start = std::chrono::steady_clock::now();
        singleTick();
        ++count;
        if (!core_.snapshot().running) {
            break;
        }
        const auto elapsed = std::chrono::steady_clock::now() - start;
        if (elapsed < frame) {
            std::this_thread::sleep_for(frame - elapsed);
        }
    }
}
