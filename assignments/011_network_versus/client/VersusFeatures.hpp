#pragma once

namespace tetris::view {

// Feature negotiation for the versus clients. The server only sends
// VERSUS_SNAPSHOT lines (both boards) when this feature is requested in
// HELLO; without it the client receives single-board SNAPSHOT lines only.
constexpr const char* VERSUS_FEATURES = "versus_snapshot";

}  // namespace tetris::view
