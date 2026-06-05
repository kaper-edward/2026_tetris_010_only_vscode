#include "NcursesVersusRenderer.hpp"
#include "NetworkClient.hpp"
#include "SfmlTcpTransport.hpp"
#include "VersusFeatures.hpp"
#include "VersusViewMapper.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <ncurses.h>
#include <string>
#include <thread>

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
};

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
        }
    }
    return args;
}

VersusViewSnapshot waiting_view(const std::string& name, ClientPhase phase) {
    VersusViewSnapshot view;
    view.phase = phase == ClientPhase::Error ? VersusViewPhase::NetworkError : VersusViewPhase::WaitingForOpponent;
    view.status_text = client_phase_name(phase);
    view.you.name = name;
    view.you.is_you = true;
    view.opponent.name = "waiting";
    return view;
}

GameAction action_from_key(int key) {
    switch (key) {
        case KEY_LEFT: return GameAction::Left;
        case KEY_RIGHT: return GameAction::Right;
        case KEY_DOWN: return GameAction::Down;
        case KEY_UP: return GameAction::Rotate;
        case ' ': return GameAction::Drop;
        case 'q':
        case 'Q':
            return GameAction::Quit;
        default:
            return GameAction::None;
    }
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
    auto transport = std::make_unique<SfmlTcpTransport>();
    if (!transport->connect(args.host, args.port, 2000)) {
        std::cerr << "failed to connect: " << args.host << ":" << args.port << "\n";
        return 2;
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    NetworkClient::setDefaultSection(args.section);
    NetworkClient::setDefaultAiLevel(args.ai_level);

    NetworkClient client(std::move(transport));
    client.connectAs(args.name, VERSUS_FEATURES);
    if (args.mode == "solo") {
        client.queueSolo();
    } else {
        client.queueVersus();
    }
    NcursesVersusRenderer renderer;

    bool running = true;
    while (running && client.phase() != ClientPhase::Error) {
        client.poll();
        const GameAction action = action_from_key(getch());
        if (action != GameAction::None) {
            client.sendAction(action);
            if (action == GameAction::Quit) {
                client.quit();
                running = false;
            }
        }
        if (auto match = client.latestRemoteMatchSnapshot()) {
            renderer.render(make_versus_view_snapshot(*match, client.phase(), client.winner(), ""));
        } else {
            renderer.render(waiting_view(args.name, client.phase()));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    endwin();
    return client.errorCount() == 0 ? 0 : 1;
}
