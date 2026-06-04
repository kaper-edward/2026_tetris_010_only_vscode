# 제공 파일 / 학생 작성 파일

이 저장소는 과제 010을 VPL에서 통과한 학생이 실제 서버에 접속해 테스트하는 용도입니다.

## 학생이 교체하는 파일

| 과제 | 파일 | 역할 |
|---|---|---|
| 010 | `assignments/010_server_client/client/RemoteRenderMapper.cpp` | 서버 snapshot을 010 단일 보드 `RenderSnapshot`으로 변환 |
| 010 | `assignments/010_server_client/client/NetworkGameApp.cpp` | SFML 입력, NetworkClient, renderer를 연결하는 클라이언트 루프 |

현재 파일들은 컴파일용 TODO 스텁입니다. 정답 코드는 이 저장소에 포함되어 있지 않습니다.

## 제공 파일

| 경로 | 내용 |
|---|---|
| `support/007_protocol/` | 서버/클라이언트 line protocol DTO와 parser |
| `support/008_transport/` | `ITransport`, `NetworkClient`, in-memory transport |
| `support/009_tcp_transport/network/LineFramer.*` | TCP line framing helper |
| `assignments/010_server_client/network/SfmlTcpTransport.*` | SFML Network 기반 TCP transport |
| `assignments/010_server_client/client/main_client_bot.cpp` | 서버 연결 smoke-test용 bot |
| `assignments/010_server_client/client/main_sfml_client.cpp` | 010 SFML client entrypoint |
| `cmake/SfmlSetup.cmake`, `CMakePresets.json`, `.vscode/` | 빌드/IDE 설정 |

재배포 권리가 명확하지 않은 asset은 이 저장소에 포함하지 않습니다.
