#ifndef TETRIS_ASSIGNMENT_011_VERSUS_VIEW_SNAPSHOT_HPP
#define TETRIS_ASSIGNMENT_011_VERSUS_VIEW_SNAPSHOT_HPP

#include "RenderSnapshot.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace tetris::view {

// View-layer id alias; avoids depending on protocol DTO headers for render state.
using ViewPlayerId = int;

struct ScoreEntryView {
    std::string name;
    int score = 0;
};

enum class VersusViewPhase {
    Connecting,
    WaitingForOpponent,
    Playing,
    GameOver,
    NetworkError
};

struct PlayerViewSnapshot {
    tetris::render::RenderSnapshot board;
    std::string name;
    ViewPlayerId player = 0;
    bool is_you = false;
};

struct VersusViewSnapshot {
    VersusViewPhase phase = VersusViewPhase::Connecting;
    PlayerViewSnapshot you;
    PlayerViewSnapshot opponent;
    std::string status_text;
    std::uint32_t tick = 0;
    int tick_rate = 20;
    ViewPlayerId winner = 0;
    std::string reason;
    bool has_high_score = false;
    int high_score = 0;
    std::vector<ScoreEntryView> top_scores;
};

const char* versus_phase_name(VersusViewPhase phase) noexcept;

}  // namespace tetris::view

#endif
