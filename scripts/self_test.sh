#!/usr/bin/env bash
# Build the public 010 SFML Network client targets and verify binaries exist.

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PRESET="${1:-default}"
BUILD_DIR="$ROOT/build/$PRESET"

cmake --preset "$PRESET"
cmake --build "$BUILD_DIR" --parallel

targets=(
  "$BUILD_DIR/assignments/010_server_client/tetris_client_bot"
  "$BUILD_DIR/assignments/010_server_client/tetris010_sfml_client"
)

for target in "${targets[@]}"; do
  if [ ! -x "$target" ]; then
    echo "missing executable: $target" >&2
    exit 1
  fi
  echo "ok: $target"
done
