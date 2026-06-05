#include "VersusViewSnapshot.hpp"

namespace tetris::view {

const char* versus_phase_name(VersusViewPhase phase) noexcept {
    switch (phase) {
        case VersusViewPhase::Connecting: return "Connecting";
        case VersusViewPhase::WaitingForOpponent: return "WaitingForOpponent";
        case VersusViewPhase::Playing: return "Playing";
        case VersusViewPhase::GameOver: return "GameOver";
        case VersusViewPhase::NetworkError: return "NetworkError";
    }
    return "NetworkError";
}

}  // namespace tetris::view
