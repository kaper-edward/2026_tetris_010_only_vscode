#include "NetworkClient.hpp"
#include "SfmlTcpTransport.hpp"
#include "VersusFeatures.hpp"
#include "SfmlVersusRenderer.hpp"
#include "VersusViewMapper.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <iostream>
#include <memory>
#include <optional>
#include <string>

using namespace tetris::net;
using namespace tetris::protocol;
using namespace tetris::view;

namespace {

struct Args {
    std::string host = "127.0.0.1";
    unsigned short port = 27015;
    std::string name = "player";
    std::string section = "062";
    int ai_level = 1;
    std::string mode = "multi";  // "multi": versus queue, "solo": instant AI match
    std::optional<std::string> font_path;
    int connect_timeout_ms = 2000;
};

std::string default_font_path() {
    return "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
}

Args parse_args(int argc, char** argv) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        const std::string key = argv[i];
        if (key == "--host" && i + 1 < argc) {
            args.host = argv[++i];
        } else if (key == "--port" && i + 1 < argc) {
            args.port = static_cast<unsigned short>(std::stoi(argv[++i]));
        } else if (key == "--name" && i + 1 < argc) {
            args.name = argv[++i];
        } else if (key == "--section" && i + 1 < argc) {
            args.section = argv[++i];
        } else if (key == "--ai-level" && i + 1 < argc) {
            args.ai_level = std::stoi(argv[++i]);
        } else if (key == "--mode" && i + 1 < argc) {
            args.mode = argv[++i];
        } else if (key == "--solo") {
            args.mode = "solo";
        } else if (key == "--font" && i + 1 < argc) {
            args.font_path = argv[++i];
        } else if (key == "--connect-timeout-ms" && i + 1 < argc) {
            args.connect_timeout_ms = std::stoi(argv[++i]);
        }
    }
    return args;
}

std::optional<GameAction> poll_action(sf::RenderWindow& window) {
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return GameAction::Quit;
        }
        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            switch (key->code) {
                case sf::Keyboard::Key::Left: return GameAction::Left;
                case sf::Keyboard::Key::Right: return GameAction::Right;
                case sf::Keyboard::Key::Down: return GameAction::Down;
                case sf::Keyboard::Key::Up: return GameAction::Rotate;
                case sf::Keyboard::Key::Space: return GameAction::Drop;
                case sf::Keyboard::Key::Escape:
                case sf::Keyboard::Key::Q:
                    window.close();
                    return GameAction::Quit;
                default: break;
            }
        }
    }
    return std::nullopt;
}

VersusViewSnapshot waiting_view(const std::string& name, ClientPhase phase) {
    VersusViewSnapshot view;
    view.phase = phase == ClientPhase::Error ? VersusViewPhase::NetworkError : VersusViewPhase::WaitingForOpponent;
    view.status_text = client_phase_name(phase);
    view.you.name = name;
    view.you.player = 0;
    view.you.is_you = true;
    view.opponent.name = "waiting";
    view.opponent.player = 0;
    for (auto& row : view.you.board.board_rows) {
        row = "..........";
    }
    for (auto& row : view.opponent.board.board_rows) {
        row = "..........";
    }
    return view;
}

}  // namespace

int main(int argc, char** argv) {
    const Args args = parse_args(argc, argv);
    if (args.mode != "solo" && args.mode != "multi") {
        std::cerr << "invalid --mode: " << args.mode << " (expected solo or multi)\n";
        return 2;
    }
    if (args.ai_level < 1 || args.ai_level > 3) {
        std::cerr << "invalid --ai-level: " << args.ai_level << " (expected 1..3)\n";
        return 2;
    }
    const std::string font_path = args.font_path.value_or(default_font_path());

    sf::RenderWindow window(sf::VideoMode({1600, 900}), "2026 Tetris Network Versus");
    window.setFramerateLimit(60);

    SfmlVersusRenderer renderer(window, font_path);
    if (!renderer.initialize()) {
        std::cerr << "failed to load font: " << font_path << "\n";
        return 2;
    }

    auto transport = std::make_unique<SfmlTcpTransport>();
    if (!transport->connect(args.host, args.port, args.connect_timeout_ms)) {
        std::cerr << "failed to connect: " << args.host << ":" << args.port << "\n";
        return 2;
    }

    NetworkClient::setDefaultSection(args.section);
    NetworkClient::setDefaultAiLevel(args.ai_level);

    NetworkClient client(std::move(transport));
    client.connectAs(args.name, VERSUS_FEATURES);
    if (args.mode == "solo") {
        client.queueSolo();
    } else {
        client.queueVersus();
    }

    std::string game_over_reason;
    while (window.isOpen() && client.phase() != ClientPhase::Error) {
        client.poll();
        if (auto action = poll_action(window)) {
            client.sendAction(*action);
            if (*action == GameAction::Quit) {
                client.quit();
                break;
            }
        }

        if (client.phase() == ClientPhase::GameOver) {
            game_over_reason = "finished";
        }

        if (auto match = client.latestRemoteMatchSnapshot()) {
            renderer.render(make_versus_view_snapshot(*match, client.phase(), client.winner(), game_over_reason, client.tickRate()));
        } else {
            renderer.render(waiting_view(args.name, client.phase()));
        }
        sf::sleep(sf::milliseconds(16));
    }

    std::cout << "welcomed=" << (client.clientId() > 0 ? 1 : 0) << "\n";
    std::cout << "match_found=" << (client.matchId() > 0 ? 1 : 0) << "\n";
    std::cout << "versus_snapshots=" << (client.latestRemoteMatchSnapshot() ? 1 : 0) << "\n";
    std::cout << "snapshots=" << client.snapshotCount() << "\n";
    std::cout << "phase=" << client_phase_name(client.phase()) << "\n";
    std::cout << "errors=" << client.errorCount() << "\n";
    return client.errorCount() == 0 ? 0 : 1;
}
