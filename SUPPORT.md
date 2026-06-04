# Support

## 자기 진단

질문 전에 아래 명령을 먼저 실행해 주세요.

```bash
cmake --preset default
cmake --build build/default --parallel
bash scripts/self_test.sh
```

서버 연결 smoke test는 다음 명령으로 확인합니다.

```bash
./build/default/assignments/010_server_client/tetris_client_bot \
  --host 158.247.241.98 \
  --port 27015 \
  --name bot010 \
  --timeout-ms 15000
```

## 질문 채널

- VPL 채점 관련: PLATO LMS 해당 실습 페이지 Q&A.
- 빌드/환경 문제: 강의 채널의 환경 도움 채널.
- 서버 접속 문제: 사용한 host, port, 실행 명령, 출력 전체를 함께 공유.
- 저장소 내용 오류: GitHub Issue 또는 강의자에게 직접 공유.

## 질문에 포함할 정보

- 운영체제와 컴파일러 버전.
- `cmake --preset default` 출력 중 마지막 에러.
- `cmake --build build/default --parallel` 출력 중 마지막 에러.
- `tetris_client_bot` 실행 출력.
- 010 TODO 파일을 본인 VPL 통과 구현으로 교체했는지 여부.

## 강의자 연락

긴급한 운영 문제만 직접 연락합니다.

- email: `pnu.ktraining.edward@gmail.com`
