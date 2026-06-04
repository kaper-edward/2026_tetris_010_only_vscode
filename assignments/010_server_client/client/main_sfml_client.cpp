#include "NetworkGameApp.hpp"
#include "SfmlInput.hpp"
#include "SfmlRenderer.hpp"
#include "SfmlTcpTransport.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>

#include <iostream>
#include <memory>
#include <string>

using namespace tetris::net;
using namespace tetris::protocol;

namespace {

struct Args {
    std::string host = "127.0.0.1";
    unsigned short port = 27015;
    std::string name = "player";
    std::string section = "062";
    std::string font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    int connect_timeout_ms = 2000;
    std::string mode = "multi";
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
        } else if (key == "--font" && i + 1 < argc) {
            args.font_path = argv[++i];
        } else if (key == "--connect-timeout-ms" && i + 1 < argc) {
            args.connect_timeout_ms = std::stoi(argv[++i]);
        } else if (key == "--mode" && i + 1 < argc) {
            args.mode = argv[++i];
        } else if (key == "--solo") {
            args.mode = "solo";
        }
    }
    return args;
}

}  // namespace

int main(int argc, char** argv) {
    const Args args = parse_args(argc, argv);
    if (args.mode != "solo" && args.mode != "multi") {
        std::cerr << "invalid --mode: " << args.mode << " (expected solo or multi)\n";
        return 2;
    }

    NetworkClient::setDefaultSection(args.section);

    sf::RenderWindow window(sf::VideoMode({760, 700}), "2026 Tetris Network Client");
    window.setFramerateLimit(60);

    SfmlRenderer renderer(window, args.font_path);
    if (!renderer.initialize()) {
        std::cerr << "failed to load font: " << args.font_path << "\n";
        return 2;
    }

    auto transport = std::make_unique<SfmlTcpTransport>();
    if (!transport->connect(args.host, args.port, args.connect_timeout_ms)) {
        std::cerr << "failed to connect: " << args.host << ":" << args.port << "\n";
        return 2;
    }

    SfmlInput input(window);
    NetworkGameApp app(std::move(transport), renderer, input, args.name, args.mode == "solo");

    while (window.isOpen()) {
        if (!app.singleFrame()) {
            break;
        }
        sf::sleep(sf::milliseconds(16));
    }

    std::cout << "welcomed=" << (app.client().clientId() > 0 ? 1 : 0) << "\n";
    std::cout << "match_found=" << (app.client().matchId() > 0 ? 1 : 0) << "\n";
    std::cout << "snapshots=" << app.client().snapshotCount() << "\n";
    std::cout << "phase=" << client_phase_name(app.client().phase()) << "\n";
    std::cout << "errors=" << app.client().errorCount() << "\n";
    return app.client().errorCount() == 0 ? 0 : 1;
}
