#include "RemoteRenderMapper.hpp"

namespace tetris::render {

RenderSnapshot make_render_snapshot_for_remote_game(const tetris::protocol::RemoteSnapshot& remote) {
    (void)remote;

    RenderSnapshot snapshot;
    for (auto& row : snapshot.board_rows) {
        row = "..........";
    }
    snapshot.running = false;
    snapshot.status_text = "TODO 009: paste your VPL-passed RemoteRenderMapper.cpp implementation";
    return snapshot;
}

}  // namespace tetris::render
