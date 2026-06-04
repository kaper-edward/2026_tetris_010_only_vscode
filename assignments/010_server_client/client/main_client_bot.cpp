#include "BotPolicy.hpp"
#include "NetworkClient.hpp"
#include "SfmlTcpTransport.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace tetris::net;
using namespace tetris::protocol;

namespace {

struct Args {
    std::string host = "127.0.0.1";
    unsigned short port = 27015;
    std::string name = "bot";
    std::string section = "062";
    int ai_level = 0;
    int timeout_ms = 15000;
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
        } else if (key == "--ai-level" && i + 1 < argc) {
            args.ai_level = std::stoi(argv[++i]);
        } else if (key == "--timeout-ms" && i + 1 < argc) {
            args.timeout_ms = std::stoi(argv[++i]);
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
    if (args.ai_level < 0 || args.ai_level > 3) {
        std::cerr << "invalid --ai-level: " << args.ai_level << " (expected 1..3)\n";
        return 2;
    }
    auto transport = std::make_unique<SfmlTcpTransport>();
    if (!transport->connect(args.host, args.port, 2000)) {
        std::cout << "connected=0\n";
        return 2;
    }

    NetworkClient client(std::move(transport));
    client.connectAs(args.name, "", args.section);
    if (args.mode == "solo") {
        client.queueSolo(args.ai_level);
    } else {
        client.queueVersus(args.ai_level);
    }

    const bool no_timeout = args.timeout_ms <= 0;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(args.timeout_ms);
    std::vector<std::unique_ptr<IBotPolicy>> policies;
    policies.push_back(std::make_unique<DropOncePolicy>());
    policies.push_back(std::make_unique<SidePolicy>(20));
    while ((no_timeout || std::chrono::steady_clock::now() < deadline) &&
           client.phase() != ClientPhase::GameOver &&
           client.phase() != ClientPhase::Error) {
        client.poll();
        if (client.matchId() > 0) {
            for (auto& policy : policies) {
                if (auto action = policy->decide(client.snapshotCount(), client.playerId())) {
                    client.sendAction(*action);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    client.poll();

    const int welcomed = client.clientId() > 0 ? 1 : 0;
    const int matched = client.matchId() > 0 ? 1 : 0;
    const int game_over = client.phase() == ClientPhase::GameOver ? 1 : 0;
    std::cout << "connected=1\n";
    std::cout << "welcomed=" << welcomed << "\n";
    std::cout << "match_found=" << matched << "\n";
    std::cout << "snapshots=" << client.snapshotCount() << "\n";
    std::cout << "game_over=" << game_over << "\n";
    std::cout << "winner=" << client.winner() << "\n";
    std::cout << "errors=" << client.errorCount() << "\n";
    return welcomed && matched && client.snapshotCount() > 0 && client.errorCount() == 0 ? 0 : 1;
}
