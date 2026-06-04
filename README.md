# 2026 C++ Tetris 010 VSCode Network Client

PNU C++ 2026 테트리스 과제 010을 VPL에서 통과한 뒤 실제 Vultr 서버에 접속해 테스트하기 위한 VSCode/CMake 프로젝트입니다.

이 저장소는 정답 저장소가 아닙니다. 아래 두 파일은 컴파일용 TODO 스텁으로 남겨 두었습니다. 학생은 본인이 VPL에서 통과시킨 구현을 붙여 넣은 뒤 서버에 접속해서 화면 테스트를 합니다.

| 과제 | 학생 구현 파일 |
|---|---|
| 010 | `assignments/010_server_client/client/RemoteRenderMapper.cpp` |
| 010 | `assignments/010_server_client/client/NetworkGameApp.cpp` |

제공 코드에는 SFML TCP transport, 프로토콜/전송 계층, 010 bot client, 010 SFML client가 포함됩니다.

## Quick Start

```bash
git clone https://github.com/kaper-edward/2026_tetris_010_only_vscode.git
cd 2026_tetris_010_only_vscode
cmake --preset default
cmake --build build/default --parallel
```

빌드 후 실행 파일은 아래 위치에 생성됩니다.

```text
build/default/assignments/010_server_client/tetris_client_bot
build/default/assignments/010_server_client/tetris010_sfml_client
```

## Server Smoke Test

`tetris_client_bot`은 TODO 스텁과 무관하게 SFML Network 연결, WELCOME, 매칭, snapshot 수신을 확인하는 용도입니다.

```bash
./build/default/assignments/010_server_client/tetris_client_bot \
  --host 158.247.241.98 \
  --port 27015 \
  --name bot010 \
  --timeout-ms 15000
```

무제한으로 실행하려면 `--timeout-ms 0`을 사용합니다.

```bash
./build/default/assignments/010_server_client/tetris_client_bot \
  --host 158.247.241.98 \
  --port 27015 \
  --name bot010 \
  --timeout-ms 0
```

## 010 SFML Client

010 VPL 통과 후 `RemoteRenderMapper.cpp`, `NetworkGameApp.cpp`를 본인 구현으로 교체하고 실행합니다.

```bash
./build/default/assignments/010_server_client/tetris010_sfml_client \
  --host 158.247.241.98 \
  --port 27015 \
  --name student010 \
  --mode solo
```

멀티 대기열로 접속하려면 `--mode multi`를 사용합니다.

```bash
./build/default/assignments/010_server_client/tetris010_sfml_client \
  --host 158.247.241.98 \
  --port 27015 \
  --name student010 \
  --mode multi
```

## SFML

`cmake/SfmlSetup.cmake`는 system SFML 3.x가 있으면 먼저 사용하고, 없으면 SFML 3.1.0을 GitHub에서 받아 빌드합니다. SFML Network가 필요하므로 Linux에서는 보통 아래 패키지가 필요합니다.

```bash
sudo apt update
sudo apt install -y cmake g++ ninja-build libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libgl1-mesa-dev libflac-dev libvorbis-dev libopenal-dev libmbedtls-dev libssh2-1-dev
```

## Repository Layout

```text
.
├── CMakeLists.txt
├── CMakePresets.json
├── .vscode/
├── cmake/SfmlSetup.cmake
├── docs/
├── scripts/self_test.sh
├── support/
└── assignments/
    └── 010_server_client/
```

`support/`에는 010 서버 클라이언트를 빌드하기 위한 protocol, transport, renderer 지원 코드가 들어 있습니다. 학생이 서버 테스트를 위해 수정해야 하는 파일은 위 표의 두 파일뿐입니다.
