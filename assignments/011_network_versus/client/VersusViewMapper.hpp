#ifndef TETRIS_ASSIGNMENT_011_VERSUS_VIEW_MAPPER_HPP
#define TETRIS_ASSIGNMENT_011_VERSUS_VIEW_MAPPER_HPP

#include "ClientPhase.hpp"
#include "RemoteSnapshot.hpp"
#include "VersusViewSnapshot.hpp"

namespace tetris::view {

tetris::render::RenderSnapshot render_snapshot_from_remote_player(const tetris::protocol::RemotePlayerSnapshot& remote);
VersusViewSnapshot make_versus_view_snapshot(
    const tetris::protocol::RemoteMatchSnapshot& remote,
    tetris::net::ClientPhase phase,
    tetris::protocol::PlayerId winner,
    const std::string& reason,
    int tick_rate = 20);

}  // namespace tetris::view

#endif
