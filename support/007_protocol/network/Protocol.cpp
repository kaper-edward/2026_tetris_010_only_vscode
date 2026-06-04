#include "Protocol.hpp"

#include <algorithm>
#include <charconv>
#include <map>
#include <sstream>
#include <vector>

namespace tetris::protocol {

namespace {

template <typename T>
bool parse_number(std::string_view text, T& value) {
    const char* first = text.data();
    const char* last = text.data() + text.size();
    const auto result = std::from_chars(first, last, value);
    return result.ec == std::errc{} && result.ptr == last;
}

std::optional<std::map<std::string, std::string>> parse_fields(std::string_view line, std::string& command) {
    if (line.size() > MAX_PROTOCOL_LINE_BYTES || line.empty()) {
        return std::nullopt;
    }

    std::istringstream input{std::string(line)};
    if (!(input >> command)) {
        return std::nullopt;
    }

    std::map<std::string, std::string> fields;
    std::string token;
    while (input >> token) {
        const std::size_t equal = token.find('=');
        if (equal == std::string::npos || equal == 0 || equal + 1 == token.size()) {
            return std::nullopt;
        }
        std::string key = token.substr(0, equal);
        std::string value = token.substr(equal + 1);
        if (!is_valid_token_value(value) || fields.count(key) != 0) {
            return std::nullopt;
        }
        fields.emplace(std::move(key), std::move(value));
    }
    return fields;
}

bool require_required_keys(const std::map<std::string, std::string>& fields, std::initializer_list<const char*> keys) {
    for (const char* key : keys) {
        if (fields.count(key) == 0) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool read_field(const std::map<std::string, std::string>& fields, const std::string& key, T& value) {
    auto it = fields.find(key);
    if (it == fields.end()) {
        return false;
    }
    return parse_number(std::string_view(it->second), value);
}

bool read_bool_field(const std::map<std::string, std::string>& fields, const std::string& key, bool& value) {
    int numeric = 0;
    if (!read_field(fields, key, numeric) || (numeric != 0 && numeric != 1)) {
        return false;
    }
    value = numeric == 1;
    return true;
}

std::vector<std::string> split_board(const std::string& board) {
    std::vector<std::string> rows;
    std::string current;
    for (char ch : board) {
        if (ch == '|') {
            rows.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }
    rows.push_back(current);
    return rows;
}

bool read_board(const std::map<std::string, std::string>& fields, const std::string& key, std::array<std::string, BOARD_HEIGHT>& board_rows) {
    auto it = fields.find(key);
    if (it == fields.end()) {
        return false;
    }
    const std::vector<std::string> rows = split_board(it->second);
    if (rows.size() != BOARD_HEIGHT) {
        return false;
    }
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        if (static_cast<int>(rows[y].size()) != BOARD_WIDTH) {
            return false;
        }
        for (char ch : rows[y]) {
            if (ch != '.' && ch != '#') {
                return false;
            }
        }
        board_rows[y] = rows[y];
    }
    return true;
}

bool read_color_board(const std::map<std::string, std::string>& fields, const std::string& key, std::array<std::string, BOARD_HEIGHT>& board_rows) {
    auto it = fields.find(key);
    if (it == fields.end()) {
        return false;
    }
    const std::vector<std::string> rows = split_board(it->second);
    if (rows.size() != BOARD_HEIGHT) {
        return false;
    }
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        if (static_cast<int>(rows[y].size()) != BOARD_WIDTH) {
            return false;
        }
        for (char ch : rows[y]) {
            const bool ok = ch == '.' || ch == 'I' || ch == 'O' || ch == 'T' ||
                            ch == 'S' || ch == 'Z' || ch == 'J' || ch == 'L' || ch == 'G';
            if (!ok) {
                return false;
            }
        }
        board_rows[y] = rows[y];
    }
    return true;
}

bool read_remote_player(const std::map<std::string, std::string>& fields, const std::string& prefix, PlayerId player, RemotePlayerSnapshot& snapshot) {
    snapshot.player = player;
    auto name_it = fields.find(prefix + "name");
    if (name_it == fields.end()) {
        return false;
    }
    snapshot.name = name_it->second;
    if (!read_bool_field(fields, prefix + "running", snapshot.running) ||
        !read_field(fields, prefix + "current", snapshot.current_tetromino) ||
        !read_field(fields, prefix + "rotation", snapshot.current_rotation) ||
        !read_field(fields, prefix + "x", snapshot.block_x) ||
        !read_field(fields, prefix + "y", snapshot.block_y) ||
        !read_field(fields, prefix + "score", snapshot.score) ||
        !read_field(fields, prefix + "level", snapshot.level) ||
        !read_field(fields, prefix + "lines", snapshot.lines_cleared) ||
        !read_field(fields, prefix + "next", snapshot.next_tetromino) ||
        !read_board(fields, prefix + "board", snapshot.board_rows)) {
        return false;
    }
    snapshot.has_next = true;
    if (fields.count(prefix + "color_board") != 0) {
        if (!read_color_board(fields, prefix + "color_board", snapshot.color_board_rows)) {
            return false;
        }
        snapshot.has_color_board = true;
    }
    if (fields.count(prefix + "ghost_y") != 0) {
        if (!read_field(fields, prefix + "ghost_y", snapshot.ghost_y)) {
            return false;
        }
        snapshot.has_ghost_y = true;
    }
    return true;
}

bool read_leaderboard(const std::map<std::string, std::string>& fields, RemoteMatchSnapshot& snapshot) {
    auto count_it = fields.find("leaderboard_count");
    if (count_it == fields.end()) {
        if (fields.count("high_score") != 0 && !read_field(fields, "high_score", snapshot.high_score)) {
            return false;
        }
        snapshot.has_high_score = fields.count("high_score") != 0;
        return true;
    }
    int count = 0;
    if (!parse_number(std::string_view(count_it->second), count) || count < 0 || count > 5) {
        return false;
    }
    snapshot.top_scores.clear();
    for (int index = 1; index <= count; ++index) {
        const std::string name_key = "top" + std::to_string(index) + "_name";
        const std::string score_key = "top" + std::to_string(index) + "_score";
        auto name_it = fields.find(name_key);
        if (name_it == fields.end()) {
            return false;
        }
        ScoreEntry entry;
        entry.name = name_it->second;
        if (!read_field(fields, score_key, entry.score)) {
            return false;
        }
        snapshot.top_scores.push_back(entry);
    }
    if (fields.count("high_score") != 0) {
        if (!read_field(fields, "high_score", snapshot.high_score)) {
            return false;
        }
        snapshot.has_high_score = true;
    }
    return true;
}

}  // namespace

std::optional<GameAction> parse_action(std::string_view text) {
    if (text == "NONE") return GameAction::None;
    if (text == "LEFT") return GameAction::Left;
    if (text == "RIGHT") return GameAction::Right;
    if (text == "DOWN") return GameAction::Down;
    if (text == "ROTATE") return GameAction::Rotate;
    if (text == "DROP") return GameAction::Drop;
    if (text == "QUIT") return GameAction::Quit;
    return std::nullopt;
}

std::string action_to_string(GameAction action) {
    switch (action) {
        case GameAction::None: return "NONE";
        case GameAction::Left: return "LEFT";
        case GameAction::Right: return "RIGHT";
        case GameAction::Down: return "DOWN";
        case GameAction::Rotate: return "ROTATE";
        case GameAction::Drop: return "DROP";
        case GameAction::Quit: return "QUIT";
    }
    return "NONE";
}

bool is_valid_token_value(std::string_view value) {
    if (value.empty()) {
        return false;
    }
    for (char ch : value) {
        const bool ok = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
                        (ch >= '0' && ch <= '9') || ch == '_' || ch == '.' ||
                        ch == ':' || ch == '#' || ch == '@' || ch == '/' ||
                        ch == '|' || ch == '-';
        if (!ok || ch == '=') {
            return false;
        }
    }
    return true;
}

std::string serialize_client_message(const ClientMessage& message) {
    std::ostringstream out;
    switch (message.type) {
        case ClientMessageType::Hello:
            out << "HELLO version=" << message.version << " name=" << message.name;
            if (!message.section.empty()) {
                out << " section=" << message.section;
            }
            if (!message.features.empty()) {
                out << " features=" << message.features;
            }
            break;
        case ClientMessageType::Queue:
            out << "QUEUE mode=" << message.mode;
            if (message.ai_level > 0) {
                out << " ai_level=" << message.ai_level;
            }
            break;
        case ClientMessageType::Input:
            out << "INPUT match=" << message.match << " player=" << message.player
                << " tick=" << message.tick << " seq=" << message.seq
                << " action=" << action_to_string(message.action);
            break;
        case ClientMessageType::Ping:
            out << "PING time=" << message.time;
            break;
        case ClientMessageType::Pong:
            out << "PONG time=" << message.time;
            break;
        case ClientMessageType::Quit:
            out << "QUIT reason=" << message.reason;
            break;
    }
    return out.str();
}

std::optional<RemoteSnapshot> parse_remote_snapshot_fields(std::string_view line) {
    std::string command;
    auto maybe_fields = parse_fields(line, command);
    if (!maybe_fields || command != "SNAPSHOT") {
        return std::nullopt;
    }
    const auto& fields = *maybe_fields;
    if (!require_required_keys(fields, {"tick", "running", "current", "rotation", "x", "y", "score", "level", "lines", "board"})) {
        return std::nullopt;
    }

    RemoteSnapshot snapshot;
    if (!read_field(fields, "tick", snapshot.tick) ||
        !read_bool_field(fields, "running", snapshot.running) ||
        !read_field(fields, "current", snapshot.current_tetromino) ||
        !read_field(fields, "rotation", snapshot.current_rotation) ||
        !read_field(fields, "x", snapshot.block_x) ||
        !read_field(fields, "y", snapshot.block_y) ||
        !read_field(fields, "score", snapshot.score) ||
        !read_field(fields, "level", snapshot.level) ||
        !read_field(fields, "lines", snapshot.lines_cleared)) {
        return std::nullopt;
    }
    if (!read_board(fields, "board", snapshot.board_rows)) return std::nullopt;
    if (fields.count("color_board") != 0) {
        if (!read_color_board(fields, "color_board", snapshot.color_board_rows)) return std::nullopt;
        snapshot.has_color_board = true;
    }
    if (fields.count("ghost_y") != 0) {
        if (!read_field(fields, "ghost_y", snapshot.ghost_y)) return std::nullopt;
        snapshot.has_ghost_y = true;
    }
    return snapshot;
}

std::optional<RemoteMatchSnapshot> parse_remote_match_snapshot_fields(std::string_view line) {
    std::string command;
    auto maybe_fields = parse_fields(line, command);
    if (!maybe_fields || command != "VERSUS_SNAPSHOT") {
        return std::nullopt;
    }
    const auto& fields = *maybe_fields;
    if (!require_required_keys(fields, {
            "match", "tick", "you", "opponent",
            "p1_name", "p1_running", "p1_current", "p1_rotation", "p1_x", "p1_y", "p1_score", "p1_level", "p1_lines", "p1_next", "p1_board",
            "p2_name", "p2_running", "p2_current", "p2_rotation", "p2_x", "p2_y", "p2_score", "p2_level", "p2_lines", "p2_next", "p2_board"})) {
        return std::nullopt;
    }

    RemoteMatchSnapshot snapshot;
    if (!read_field(fields, "match", snapshot.match) ||
        !read_field(fields, "tick", snapshot.tick) ||
        !read_field(fields, "you", snapshot.you) ||
        !read_field(fields, "opponent", snapshot.opponent)) {
        return std::nullopt;
    }
    if ((snapshot.you != 1 && snapshot.you != 2) || (snapshot.opponent != 1 && snapshot.opponent != 2) || snapshot.you == snapshot.opponent) {
        return std::nullopt;
    }
    if (!read_remote_player(fields, "p1_", 1, snapshot.players[0]) ||
        !read_remote_player(fields, "p2_", 2, snapshot.players[1])) {
        return std::nullopt;
    }
    if (!read_leaderboard(fields, snapshot)) {
        return std::nullopt;
    }
    return snapshot;
}

std::optional<ServerMessage> parse_server_message(std::string_view line) {
    std::string command;
    auto maybe_fields = parse_fields(line, command);
    if (!maybe_fields) {
        return std::nullopt;
    }
    const auto& fields = *maybe_fields;
    ServerMessage message;

    if (command == "WELCOME") {
        if (!require_required_keys(fields, {"client", "name"}) || !read_field(fields, "client", message.client)) return std::nullopt;
        message.type = ServerMessageType::Welcome;
        message.name = fields.at("name");
        read_field(fields, "tick_rate", message.tick_rate);
        read_field(fields, "snapshot_rate", message.snapshot_rate);
        if (fields.count("protocol") != 0) {
            int parsed = 0;
            if (!read_field(fields, "protocol", parsed) || parsed < 1) {
                return std::nullopt;
            }
            message.has_protocol_version = true;
            message.protocol_version = parsed;
        }
        return message;
    }
    if (command == "QUEUE_STATUS") {
        if (!require_required_keys(fields, {"position", "estimated_wait"}) ||
            !read_field(fields, "position", message.position) ||
            !read_field(fields, "estimated_wait", message.estimated_wait)) {
            return std::nullopt;
        }
        message.type = ServerMessageType::QueueStatus;
        return message;
    }
    if (command == "MATCH_FOUND") {
        if (!require_required_keys(fields, {"match", "player", "opponent", "seed", "tick_rate"}) ||
            !read_field(fields, "match", message.match) ||
            !read_field(fields, "player", message.player) ||
            !read_field(fields, "seed", message.seed) ||
            !read_field(fields, "tick_rate", message.tick_rate)) {
            return std::nullopt;
        }
        message.type = ServerMessageType::MatchFound;
        message.opponent = fields.at("opponent");
        return message;
    }
    if (command == "PLAYER_STATUS") {
        if (!require_required_keys(fields, {"player", "connected", "lag_ms", "last_seq"}) ||
            !read_field(fields, "player", message.player) ||
            !read_bool_field(fields, "connected", message.connected) ||
            !read_field(fields, "lag_ms", message.lag_ms) ||
            !read_field(fields, "last_seq", message.last_seq)) {
            return std::nullopt;
        }
        message.type = ServerMessageType::PlayerStatus;
        return message;
    }
    if (command == "INPUT_ACK") {
        if (!require_required_keys(fields, {"match", "player", "seq", "accepted_tick", "status", "reason"}) ||
            !read_field(fields, "match", message.match) ||
            !read_field(fields, "player", message.player) ||
            !read_field(fields, "seq", message.seq) ||
            !read_field(fields, "accepted_tick", message.accepted_tick)) {
            return std::nullopt;
        }
        message.type = ServerMessageType::InputAck;
        message.status = fields.at("status");
        message.reason = fields.at("reason");
        return message;
    }
    if (command == "SNAPSHOT") {
        auto snapshot = parse_remote_snapshot_fields(line);
        if (!snapshot) return std::nullopt;
        message.type = ServerMessageType::Snapshot;
        message.remote_snapshot = *snapshot;
        return message;
    }
    if (command == "VERSUS_SNAPSHOT") {
        auto snapshot = parse_remote_match_snapshot_fields(line);
        if (!snapshot) return std::nullopt;
        message.type = ServerMessageType::VersusSnapshot;
        message.remote_match_snapshot = *snapshot;
        return message;
    }
    if (command == "MATCH_SUMMARY") {
        if (!require_required_keys(fields, {"match", "winner", "reason", "duration_ms", "ticks", "inputs_hash", "final_hash"}) ||
            !read_field(fields, "match", message.match) ||
            !read_field(fields, "winner", message.winner) ||
            !read_field(fields, "duration_ms", message.duration_ms) ||
            !read_field(fields, "ticks", message.ticks)) {
            return std::nullopt;
        }
        message.type = ServerMessageType::MatchSummary;
        message.reason = fields.at("reason");
        message.inputs_hash = fields.at("inputs_hash");
        message.final_hash = fields.at("final_hash");
        return message;
    }
    if (command == "SERVER_NOTICE") {
        if (!require_required_keys(fields, {"severity", "code", "message"})) return std::nullopt;
        message.type = ServerMessageType::ServerNotice;
        message.severity = fields.at("severity");
        message.code = fields.at("code");
        message.message = fields.at("message");
        return message;
    }
    if (command == "GAME_OVER") {
        if (!require_required_keys(fields, {"match", "winner", "reason"}) ||
            !read_field(fields, "match", message.match) ||
            !read_field(fields, "winner", message.winner)) {
            return std::nullopt;
        }
        message.type = ServerMessageType::GameOver;
        message.reason = fields.at("reason");
        return message;
    }
    if (command == "ERROR") {
        if (!require_required_keys(fields, {"code", "message"})) return std::nullopt;
        message.type = ServerMessageType::Error;
        message.code = fields.at("code");
        message.message = fields.at("message");
        return message;
    }
    if (command == "PING" || command == "PONG") {
        if (!require_required_keys(fields, {"time"}) || !read_field(fields, "time", message.time)) return std::nullopt;
        message.type = command == "PING" ? ServerMessageType::Ping : ServerMessageType::Pong;
        return message;
    }
    message.type = ServerMessageType::Unknown;
    message.message = command;
    return message;
}

}  // namespace tetris::protocol
