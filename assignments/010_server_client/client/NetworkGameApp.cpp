#include "NetworkGameApp.hpp"

#include <chrono>
#include <thread>
#include <utility>

NetworkGameApp::NetworkGameApp(std::unique_ptr<tetris::net::ITransport> transport,
                               IRenderer& renderer,
                               IInput& input,
                               std::string name,
                               bool solo_mode)
    : client_(std::move(transport)), renderer_(renderer), input_(input), name_(std::move(name)) {
    (void)solo_mode;
}

bool NetworkGameApp::singleFrame() {
    client_.poll();
    if (auto action = input_.pollAction()) {
        if (*action == GameAction::Quit) {
            client_.quit();
            return false;
        }
    }
    renderer_.render(makeCurrentRenderSnapshot());
    return true;
}

void NetworkGameApp::run(int frame_delay_ms) {
    while (singleFrame()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(frame_delay_ms));
    }
}

const tetris::net::NetworkClient& NetworkGameApp::client() const {
    return client_;
}

tetris::render::RenderSnapshot NetworkGameApp::makeCurrentRenderSnapshot() const {
    tetris::render::RenderSnapshot snapshot;
    for (auto& row : snapshot.board_rows) {
        row = "..........";
    }
    snapshot.running = false;
    snapshot.status_text = statusText();
    return snapshot;
}

std::string NetworkGameApp::statusText() const {
    return "TODO 009: paste your VPL-passed NetworkGameApp.cpp implementation";
}
