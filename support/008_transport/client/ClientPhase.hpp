#ifndef TETRIS_ASSIGNMENT_008_CLIENT_PHASE_HPP
#define TETRIS_ASSIGNMENT_008_CLIENT_PHASE_HPP

namespace tetris::net {

enum class ClientPhase {
    Disconnected,
    Connected,
    Welcomed,
    Queued,
    Matched,
    Playing,
    GameOver,
    Error
};

inline const char* client_phase_name(ClientPhase phase) noexcept {
    switch (phase) {
        case ClientPhase::Disconnected: return "Disconnected";
        case ClientPhase::Connected: return "Connected";
        case ClientPhase::Welcomed: return "Welcomed";
        case ClientPhase::Queued: return "Queued";
        case ClientPhase::Matched: return "Matched";
        case ClientPhase::Playing: return "Playing";
        case ClientPhase::GameOver: return "GameOver";
        case ClientPhase::Error: return "Error";
    }
    return "Error";
}

}  // namespace tetris::net

#endif
