#ifndef TETRIS_ASSIGNMENT_010_NETWORK_GAME_APP_HPP
#define TETRIS_ASSIGNMENT_010_NETWORK_GAME_APP_HPP

#include "IInput.hpp"
#include "IRenderer.hpp"
#include "ITransport.hpp"
#include "NetworkClient.hpp"

#include <memory>
#include <string>

class NetworkGameApp {
public:
    NetworkGameApp(std::unique_ptr<tetris::net::ITransport> transport, IRenderer& renderer, IInput& input, std::string name, bool solo_mode = false);

    NetworkGameApp(const NetworkGameApp&) = delete;
    NetworkGameApp& operator=(const NetworkGameApp&) = delete;
    NetworkGameApp(NetworkGameApp&&) = delete;
    NetworkGameApp& operator=(NetworkGameApp&&) = delete;

    bool singleFrame();
    void run(int frame_delay_ms = 16);

    const tetris::net::NetworkClient& client() const;

private:
    tetris::net::NetworkClient client_;
    IRenderer& renderer_;  // non-owning; must outlive NetworkGameApp
    IInput& input_;        // non-owning; must outlive NetworkGameApp
    std::string name_;

    tetris::render::RenderSnapshot makeCurrentRenderSnapshot() const;
    std::string statusText() const;
};

#endif
