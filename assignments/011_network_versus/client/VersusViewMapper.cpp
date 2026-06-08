#include "VersusViewMapper.hpp"

namespace tetris::view {

tetris::render::RenderSnapshot render_snapshot_from_remote_player(const tetris::protocol::RemotePlayerSnapshot& /*remote*/) {
    tetris::render::RenderSnapshot render;
    for (auto& row : render.board_rows) {
        row = "..........";
    }
    render.status_text = "TODO 010: paste your VPL-passed VersusViewMapper.cpp implementation";
    return render;
}

VersusViewSnapshot make_versus_view_snapshot(
    const tetris::protocol::RemoteMatchSnapshot& /*remote*/,
    tetris::net::ClientPhase /*phase*/,
    tetris::protocol::PlayerId /*winner*/,
    const std::string& /*reason*/,
    int tick_rate) {
    VersusViewSnapshot view;
    view.phase = VersusViewPhase::WaitingForOpponent;
    view.tick_rate = tick_rate > 0 ? tick_rate : 20;
    view.status_text = "TODO 010: paste your VPL-passed VersusViewMapper.cpp implementation";
    view.you.board = render_snapshot_from_remote_player({});
    view.opponent.board = render_snapshot_from_remote_player({});
    return view;
}

}  // namespace tetris::view
