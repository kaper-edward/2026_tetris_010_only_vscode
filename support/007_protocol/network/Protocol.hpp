#ifndef TETRIS_ASSIGNMENT_007_PROTOCOL_HPP
#define TETRIS_ASSIGNMENT_007_PROTOCOL_HPP

#include "GameAction.hpp"
#include "RemoteSnapshot.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace tetris::protocol {

using ClientId = int;
using Tick = std::uint32_t;

constexpr std::size_t MAX_PROTOCOL_LINE_BYTES = 8192;

enum class ClientMessageType {
    Hello,
    Queue,
    Input,
    Ping,
    Pong,
    Quit
};

enum class ServerMessageType {
    Unknown,
    Welcome,
    MatchFound,
    Snapshot,
    VersusSnapshot,
    InputAck,
    QueueStatus,
    PlayerStatus,
    MatchSummary,
    ServerNotice,
    GameOver,
    Error,
    Ping,
    Pong
};

struct ClientMessage {
    ClientMessageType type = ClientMessageType::Hello;
    int version = 1;
    std::string name;
    std::string features;
    std::string mode = "versus";
    MatchId match = 0;
    PlayerId player = 0;
    Tick tick = 0;
    int seq = 0;
    GameAction action = GameAction::None;
    std::uint64_t time = 0;
    std::string reason = "user";
};

// Flat DTO — fields below are a *union by convention* across ServerMessageType variants
// (Welcome uses client/name/has_protocol_version/protocol_version/tick_rate/snapshot_rate;
// MatchFound uses match/player/opponent/seed/tick_rate; Snapshot uses remote_snapshot; ...).
// Reading a non-applicable field after parse yields the default-initialized value, which
// is safe but semantically meaningless. Treat fields as valid only for the matching `type`.
struct ServerMessage {
    ServerMessageType type = ServerMessageType::Welcome;
    ClientId client = 0;
    std::string name;
    MatchId match = 0;
    PlayerId player = 0;
    std::string opponent;
    std::uint32_t seed = 0;
    int tick_rate = 0;
    int snapshot_rate = 0;
    bool has_protocol_version = false;
    int protocol_version = 1;
    RemoteSnapshot remote_snapshot;
    RemoteMatchSnapshot remote_match_snapshot;
    int position = 0;
    int estimated_wait = 0;
    Tick accepted_tick = 0;
    int seq = 0;
    std::string status;
    bool connected = false;
    int lag_ms = 0;
    int last_seq = 0;
    PlayerId winner = 0;
    int duration_ms = 0;
    int ticks = 0;
    std::string inputs_hash;
    std::string final_hash;
    std::string severity;
    std::string reason;
    std::string code;
    std::string message;
    std::uint64_t time = 0;
};

std::optional<GameAction> parse_action(std::string_view text);
std::string action_to_string(GameAction action);
bool is_valid_token_value(std::string_view value);

std::string serialize_client_message(const ClientMessage& message);
std::optional<ServerMessage> parse_server_message(std::string_view line);
std::optional<RemoteSnapshot> parse_remote_snapshot_fields(std::string_view line);
std::optional<RemoteMatchSnapshot> parse_remote_match_snapshot_fields(std::string_view line);

}  // namespace tetris::protocol

#endif
