#include "NetworkClient.hpp"

#include <utility>

namespace tetris::net {

using tetris::protocol::ClientMessage;
using tetris::protocol::ClientMessageType;
using tetris::protocol::MatchId;
using tetris::protocol::PlayerId;
using tetris::protocol::RemoteMatchSnapshot;
using tetris::protocol::RemoteSnapshot;
using tetris::protocol::ServerMessage;
using tetris::protocol::ServerMessageType;
using tetris::protocol::Tick;
using tetris::protocol::action_to_string;
using tetris::protocol::is_valid_token_value;
using tetris::protocol::parse_server_message;
using tetris::protocol::serialize_client_message;

namespace {

bool is_terminal_phase(ClientPhase phase) noexcept {
    return phase == ClientPhase::Error || phase == ClientPhase::GameOver;
}

bool optional_token_valid(const std::string& value) {
    return value.empty() || is_valid_token_value(value);
}

bool client_message_tokens_valid(const ClientMessage& message) {
    switch (message.type) {
        case ClientMessageType::Hello:
            return is_valid_token_value(message.name) && optional_token_valid(message.features);
        case ClientMessageType::Queue:
            return is_valid_token_value(message.mode);
        case ClientMessageType::Quit:
            return is_valid_token_value(message.reason);
        case ClientMessageType::Input:
        case ClientMessageType::Ping:
        case ClientMessageType::Pong:
            return true;
    }
    return false;
}

}  // namespace

NetworkClient::NetworkClient(std::unique_ptr<ITransport> transport) : transport_(std::move(transport)) {}

void NetworkClient::connectAs(const std::string& name) {
    connectAs(name, "");
}

void NetworkClient::connectAs(const std::string& name, const std::string& features) {
    if (is_terminal_phase(phase_)) {
        return;
    }
    ClientMessage message;
    message.type = ClientMessageType::Hello;
    message.version = features.empty() ? 1 : 2;
    message.name = name;
    message.features = features;
    if (!send(message)) {
        return;
    }
    name_ = name;
    phase_ = ClientPhase::Connected;
}

void NetworkClient::queueVersus() {
    if (is_terminal_phase(phase_)) {
        return;
    }
    ClientMessage message;
    message.type = ClientMessageType::Queue;
    message.mode = "versus";
    if (!send(message)) {
        return;
    }
    if (phase_ == ClientPhase::Welcomed || phase_ == ClientPhase::Connected) {
        phase_ = ClientPhase::Queued;
    }
}

void NetworkClient::queueSolo() {
    if (is_terminal_phase(phase_)) {
        return;
    }
    ClientMessage message;
    message.type = ClientMessageType::Queue;
    message.mode = "solo";
    if (!send(message)) {
        return;
    }
    if (phase_ == ClientPhase::Welcomed || phase_ == ClientPhase::Connected) {
        phase_ = ClientPhase::Queued;
    }
}

void NetworkClient::sendAction(GameAction action) {
    if (match_id_ == 0 || player_id_ == 0 || is_terminal_phase(phase_)) {
        return;
    }
    ClientMessage message;
    message.type = ClientMessageType::Input;
    message.match = match_id_;
    message.player = player_id_;
    message.tick = latest_tick_;
    message.seq = next_seq_++;
    message.action = action;
    send(message);
    structured_events_.push_back({ClientEventType::InputSent, message.tick, message.seq, action, action_to_string(action)});
}

void NetworkClient::quit() {
    if (is_terminal_phase(phase_)) {
        return;
    }
    ClientMessage message;
    message.type = ClientMessageType::Quit;
    message.reason = "user";
    send(message);
}

void NetworkClient::poll() {
    if (is_terminal_phase(phase_)) {
        return;
    }
    while (auto line = transport_->receive()) {
        auto parsed = parse_server_message(*line);
        if (!parsed) {
            enterError("bad_message", "bad_message");
            break;
        }
        handle(*parsed);
        if (is_terminal_phase(phase_)) {
            break;
        }
    }
    if (transport_->disconnected() && !is_terminal_phase(phase_)) {
        enterError("transport_disconnected", "transport_disconnected");
    }
}

ClientPhase NetworkClient::phase() const noexcept {
    return phase_;
}

int NetworkClient::clientId() const noexcept {
    return client_id_;
}

const std::string& NetworkClient::name() const noexcept {
    return name_;
}

MatchId NetworkClient::matchId() const noexcept {
    return match_id_;
}

PlayerId NetworkClient::playerId() const noexcept {
    return player_id_;
}

PlayerId NetworkClient::winner() const noexcept {
    return winner_;
}

Tick NetworkClient::latestTick() const noexcept {
    return latest_tick_;
}

int NetworkClient::tickRate() const noexcept {
    return tick_rate_;
}

int NetworkClient::protocolVersion() const noexcept {
    return protocol_version_;
}

bool NetworkClient::hasProtocolVersion() const noexcept {
    return has_protocol_version_;
}

int NetworkClient::snapshotCount() const noexcept {
    return snapshot_count_;
}

int NetworkClient::errorCount() const noexcept {
    return error_count_;
}

std::optional<RemoteSnapshot> NetworkClient::latestRemoteSnapshot() const {
    return latest_remote_snapshot_;
}

std::optional<RemoteMatchSnapshot> NetworkClient::latestRemoteMatchSnapshot() const {
    return latest_remote_match_snapshot_;
}

const std::vector<std::string>& NetworkClient::events() const {
    return events_;
}

const std::vector<ClientEvent>& NetworkClient::structuredEvents() const {
    return structured_events_;
}

std::vector<ClientEvent> NetworkClient::drainStructuredEvents() {
    std::vector<ClientEvent> events;
    events.swap(structured_events_);
    return events;
}

bool NetworkClient::send(const ClientMessage& message) {
    if (!client_message_tokens_valid(message)) {
        enterError("bad_client_token", "bad_client_token");
        return false;
    }
    transport_->send(serialize_client_message(message));
    return true;
}

void NetworkClient::handle(const ServerMessage& message) {
    if (is_terminal_phase(phase_)) {
        return;
    }
    switch (message.type) {
        case ServerMessageType::Unknown:
            events_.push_back("unknown_message:" + message.message);
            structured_events_.push_back({ClientEventType::UnknownMessage, latest_tick_, 0, GameAction::None, message.message});
            break;
        case ServerMessageType::Welcome:
            client_id_ = message.client;
            if (message.tick_rate > 0) {
                tick_rate_ = message.tick_rate;
            }
            // Always overwrite — even a missing field resets to v1 default so that
            // a v2-server WELCOME followed by a v1-server WELCOME does not leak v2 state.
            // See docs/protocol_evolution_policy.md §2 (default-to-v1 rule).
            protocol_version_ = message.protocol_version;
            has_protocol_version_ = message.has_protocol_version;
            phase_ = ClientPhase::Welcomed;
            events_.push_back("welcome");
            structured_events_.push_back({ClientEventType::Welcome, latest_tick_, 0, GameAction::None, message.name});
            break;
        case ServerMessageType::MatchFound:
            match_id_ = message.match;
            player_id_ = message.player;
            if (message.tick_rate > 0) {
                tick_rate_ = message.tick_rate;
            }
            next_seq_ = 1;
            latest_tick_ = 0;
            phase_ = ClientPhase::Matched;
            events_.push_back("match_found");
            structured_events_.push_back({ClientEventType::MatchFound, latest_tick_, 0, GameAction::None, message.opponent});
            break;
        case ServerMessageType::InputAck:
            events_.push_back("input_ack:" + message.status);
            structured_events_.push_back({ClientEventType::InputAck, message.accepted_tick, message.seq, GameAction::None, message.status});
            break;
        case ServerMessageType::QueueStatus:
            events_.push_back("queue_status:" + std::to_string(message.position));
            structured_events_.push_back({ClientEventType::QueueStatus, latest_tick_, 0, GameAction::None, std::to_string(message.position)});
            break;
        case ServerMessageType::PlayerStatus:
            events_.push_back("player_status:" + std::to_string(message.player));
            structured_events_.push_back({ClientEventType::PlayerStatus, latest_tick_, message.last_seq, GameAction::None, std::to_string(message.player)});
            break;
        case ServerMessageType::MatchSummary:
            events_.push_back("match_summary:" + message.reason);
            structured_events_.push_back({ClientEventType::MatchSummary, static_cast<Tick>(message.ticks), 0, GameAction::None, message.final_hash});
            break;
        case ServerMessageType::ServerNotice:
            events_.push_back("server_notice:" + message.code);
            structured_events_.push_back({ClientEventType::ServerNotice, latest_tick_, 0, GameAction::None, message.code});
            break;
        case ServerMessageType::Snapshot:
            latest_remote_snapshot_ = message.remote_snapshot;
            latest_tick_ = message.remote_snapshot.tick;
            ++snapshot_count_;
            structured_events_.push_back({ClientEventType::SnapshotReceived, latest_tick_, 0, GameAction::None, ""});
            if (phase_ == ClientPhase::Matched || phase_ == ClientPhase::Queued || phase_ == ClientPhase::Welcomed) {
                phase_ = ClientPhase::Playing;
            }
            break;
        case ServerMessageType::VersusSnapshot:
            latest_remote_match_snapshot_ = message.remote_match_snapshot;
            latest_tick_ = message.remote_match_snapshot.tick;
            ++snapshot_count_;
            structured_events_.push_back({ClientEventType::VersusSnapshotReceived, latest_tick_, 0, GameAction::None, ""});
            if (phase_ == ClientPhase::Matched || phase_ == ClientPhase::Queued || phase_ == ClientPhase::Welcomed) {
                phase_ = ClientPhase::Playing;
            }
            break;
        case ServerMessageType::GameOver:
            winner_ = message.winner;
            phase_ = ClientPhase::GameOver;
            events_.push_back("game_over");
            structured_events_.push_back({ClientEventType::GameOver, latest_tick_, 0, GameAction::None, message.reason});
            break;
        case ServerMessageType::Error:
            enterError(message.code, "error:" + message.code);
            break;
        case ServerMessageType::Ping: {
            ClientMessage pong;
            pong.type = ClientMessageType::Pong;
            pong.time = message.time;
            send(pong);
            structured_events_.push_back({ClientEventType::PongSent, latest_tick_, 0, GameAction::None, std::to_string(message.time)});
            break;
        }
        case ServerMessageType::Pong:
            events_.push_back("pong");
            break;
    }
}

void NetworkClient::enterError(const std::string& detail, const std::string& event) {
    if (phase_ == ClientPhase::Error) {
        return;
    }
    ++error_count_;
    phase_ = ClientPhase::Error;
    events_.push_back(event);
    structured_events_.push_back({ClientEventType::Error, latest_tick_, 0, GameAction::None, detail});
}

}  // namespace tetris::net
