# Assignment 010: Server-Connected Client

Goal: connect the C++ client to `/home/edward/dev/2026_tetris_server`.

Default tests are in-memory and deterministic. SFML Network/Graphics targets
are optional:

```bash
cmake --preset default
cmake --build build/default -j
```

The optional build creates two executables:

- `tetris_client_bot`: non-interactive compatibility bot.
- `tetris010_sfml_client`: interactive SFML client for human play.

The bot is a scripted smoke-test helper. Its behavior is factored through
`IBotPolicy`, `DropOncePolicy`, and `SidePolicy`, but it is not an AI player and
does not inspect the board to choose placements.

Against the classroom server you usually do not need this local bot. Both clients
take `--mode solo|multi` (default `multi`), `--section 061|062` (default
`062`), and optional `--ai-level 1|2|3`:

- `--mode solo` sends `QUEUE mode=solo`; the server attaches a server-side bot
  opponent immediately.
- `--mode multi` sends `QUEUE mode=versus`; the server matches a human first and
  autofills a server bot after ~5s (classroom config) if none arrives.
- `--section 061|062` selects the Top20 classroom bucket. Section `062` is the
  default; section `061` students must pass `--section 061`.
- `--ai-level 1|2|3` selects the server bot difficulty when a bot opponent is
  attached. If omitted, the server uses Lv1.

When the opponent is a server bot its name shows as `cpu_<client_id>`. The CLI
flag `multi` maps to the wire word `versus`, so the client does not accept
`--mode versus`. See `2026_tetris_server/docs/student_client_guide.md` for the
canonical connection guide.

010 clients use the compatibility handshake when no section is configured:

```text
HELLO version=1 name=<name>
```

The provided classroom executables configure a section and send:

```text
HELLO version=2 name=<name> section=<061|062>
QUEUE mode=<solo|versus> ai_level=<1|2|3>   # only when --ai-level is passed
```

This target builds SFML Network, Window, and Graphics. The CMake configuration
uses SFML's bundled dependency flow for network TLS dependencies so the course
machine does not need a system MbedTLS package.

Important architecture rule:

```text
RemoteSnapshot -> make_render_snapshot_for_remote_game() -> RenderSnapshot -> renderer
```

This is the assignment where gameplay authority moves from the local C++
`GameCore` to the classroom server. The client sends `INPUT` messages and
renders the server snapshot; it does not run a local prediction or overlay an
active piece from `current`, `rotation`, `x`, and `y`.

The assignment 010 compatibility `SNAPSHOT` does not provide next-piece preview
data, so remote render snapshots set `next_piece.available=false`.

The interactive client must send keyboard actions through
`NetworkClient::sendAction()`. It must not construct `INPUT` protocol lines
directly because `NetworkClient` owns `match`, `player`, `tick`, and `seq`.

Network play does not use local `GameCore`; the classroom server is the
authoritative game core.
