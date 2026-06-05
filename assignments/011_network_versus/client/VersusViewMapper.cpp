#include "VersusViewMapper.hpp"

#include "TetrominoPreview.hpp"

namespace tetris::view {

using tetris::net::ClientPhase;
using tetris::protocol::PlayerId;
using tetris::protocol::RemoteMatchSnapshot;
using tetris::protocol::RemotePlayerSnapshot;
using tetris::render::RenderSnapshot;
using tetris::render::tetromino_preview_rows;

namespace {

const RemotePlayerSnapshot& player_for_id(const RemoteMatchSnapshot& remote, PlayerId player) {
    return remote.players[player == 2 ? 1 : 0];
}

VersusViewPhase map_phase(ClientPhase phase, PlayerId winner) {
    if (phase == ClientPhase::Error) {
        return VersusViewPhase::NetworkError;
    }
    if (phase == ClientPhase::GameOver || winner != 0) {
        return VersusViewPhase::GameOver;
    }
    if (phase == ClientPhase::Queued || phase == ClientPhase::Welcomed || phase == ClientPhase::Connected) {
        return VersusViewPhase::WaitingForOpponent;
    }
    return VersusViewPhase::Playing;
}

}  // namespace

RenderSnapshot render_snapshot_from_remote_player(const RemotePlayerSnapshot& remote) {
    RenderSnapshot render;
    render.tick = 0;
    render.board_rows = remote.board_rows;
    render.has_color_board = remote.has_color_board;
    render.color_board_rows = remote.color_board_rows;
    render.current_tetromino = remote.current_tetromino;
    render.current_rotation = remote.current_rotation;
    render.block_x = remote.block_x;
    render.block_y = remote.block_y;
    render.has_ghost_y = remote.has_ghost_y;
    render.ghost_y = remote.ghost_y;
    render.score = remote.score;
    render.level = remote.level;
    render.lines_cleared = remote.lines_cleared;
    render.running = remote.running;
    if (remote.has_next) {
        render.next_piece.available = true;
        render.next_piece.tetromino = remote.next_tetromino;
        render.next_piece.rows = tetromino_preview_rows(remote.next_tetromino);
    }
    return render;
}

VersusViewSnapshot make_versus_view_snapshot(const RemoteMatchSnapshot& remote, ClientPhase phase, PlayerId winner, const std::string& reason, int tick_rate) {
    VersusViewSnapshot view;
    view.phase = map_phase(phase, winner);
    view.tick = remote.tick;
    view.tick_rate = tick_rate > 0 ? tick_rate : 20;
    view.winner = winner;
    view.reason = reason;
    view.has_high_score = remote.has_high_score;
    view.high_score = remote.high_score;
    for (const auto& entry : remote.top_scores) {
        view.top_scores.push_back({entry.name, entry.score});
    }

    const RemotePlayerSnapshot& you = player_for_id(remote, remote.you);
    const RemotePlayerSnapshot& opponent = player_for_id(remote, remote.opponent);
    view.you.board = render_snapshot_from_remote_player(you);
    view.you.name = you.name;
    view.you.player = you.player;
    view.you.is_you = true;
    view.opponent.board = render_snapshot_from_remote_player(opponent);
    view.opponent.name = opponent.name;
    view.opponent.player = opponent.player;
    view.opponent.is_you = false;

    if (view.phase == VersusViewPhase::GameOver) {
        view.status_text = "Game over";
    } else if (view.phase == VersusViewPhase::NetworkError) {
        view.status_text = "Network error";
    } else {
        view.status_text = "Playing";
    }
    return view;
}

}  // namespace tetris::view
