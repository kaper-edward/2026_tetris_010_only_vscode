#ifndef TETRIS_ASSIGNMENT_010_REMOTE_RENDER_MAPPER_HPP
#define TETRIS_ASSIGNMENT_010_REMOTE_RENDER_MAPPER_HPP

#include "RemoteSnapshot.hpp"
#include "RenderSnapshot.hpp"

namespace tetris::render {

RenderSnapshot make_render_snapshot_for_remote_game(const tetris::protocol::RemoteSnapshot& remote);

}  // namespace tetris::render

#endif
