#ifndef TETRIS_ASSIGNMENT_008_NETWORK_CLIENT_HPP
#define TETRIS_ASSIGNMENT_008_NETWORK_CLIENT_HPP

#include "ClientPhase.hpp"
#include "ITransport.hpp"
#include "Protocol.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace tetris::net {

enum class ClientEventType {
    Welcome,
    MatchFound,
    InputAck,
    QueueStatus,
    PlayerStatus,
    MatchSummary,
    ServerNotice,
    InputSent,
    SnapshotReceived,
    VersusSnapshotReceived,
    GameOver,
    Error,
    UnknownMessage,
    PongSent
};

struct ClientEvent {
    ClientEventType type = ClientEventType::Welcome;
    tetris::protocol::Tick tick = 0;
    int seq = 0;
    tetris::protocol::GameAction action = tetris::protocol::GameAction::None;
    std::string detail;
};

class NetworkClient {
public:
    explicit NetworkClient(std::unique_ptr<ITransport> transport);

    NetworkClient(const NetworkClient&) = delete;
    NetworkClient& operator=(const NetworkClient&) = delete;
    NetworkClient(NetworkClient&&) noexcept = default;
    NetworkClient& operator=(NetworkClient&&) noexcept = default;

    static void setDefaultSection(std::string section);
    static const std::string& defaultSection();

    void connectAs(const std::string& name);
    void connectAs(const std::string& name, const std::string& features);
    void connectAs(const std::string& name, const std::string& features, const std::string& section);
    void queueVersus();
    void queueSolo();
    void sendAction(tetris::protocol::GameAction action);
    void quit();
    void poll();

    ClientPhase phase() const noexcept;
    int clientId() const noexcept;
    const std::string& name() const noexcept;
    tetris::protocol::MatchId matchId() const noexcept;
    tetris::protocol::PlayerId playerId() const noexcept;
    tetris::protocol::PlayerId winner() const noexcept;
    tetris::protocol::Tick latestTick() const noexcept;
    int tickRate() const noexcept;
    // Negotiated wire protocol version per docs/protocol_evolution_policy.md. Meaningful
    // only after a WELCOME has been observed (phase >= Welcomed). Reset to v1 on every
    // WELCOME — a v2-then-v1 reconnect must not leak v2 state into the v1 session.
    int protocolVersion() const noexcept;
    bool hasProtocolVersion() const noexcept;  // false until a WELCOME with protocol=… field arrives
    int snapshotCount() const noexcept;
    int errorCount() const noexcept;
    std::optional<tetris::protocol::RemoteSnapshot> latestRemoteSnapshot() const;
    std::optional<tetris::protocol::RemoteMatchSnapshot> latestRemoteMatchSnapshot() const;
    const std::vector<std::string>& events() const;
    const std::vector<ClientEvent>& structuredEvents() const;
    std::vector<ClientEvent> drainStructuredEvents();

private:
    bool send(const tetris::protocol::ClientMessage& message);
    void handle(const tetris::protocol::ServerMessage& message);
    void enterError(const std::string& detail, const std::string& event);

    std::unique_ptr<ITransport> transport_;
    ClientPhase phase_ = ClientPhase::Disconnected;
    int client_id_ = 0;
    std::string name_;
    tetris::protocol::MatchId match_id_ = 0;
    tetris::protocol::PlayerId player_id_ = 0;
    tetris::protocol::PlayerId winner_ = 0;
    tetris::protocol::Tick latest_tick_ = 0;
    int tick_rate_ = 20;
    int protocol_version_ = 1;
    bool has_protocol_version_ = false;
    int next_seq_ = 1;
    int snapshot_count_ = 0;
    int error_count_ = 0;
    std::optional<tetris::protocol::RemoteSnapshot> latest_remote_snapshot_;
    std::optional<tetris::protocol::RemoteMatchSnapshot> latest_remote_match_snapshot_;
    std::vector<std::string> events_;
    std::vector<ClientEvent> structured_events_;
};

}  // namespace tetris::net

#endif
