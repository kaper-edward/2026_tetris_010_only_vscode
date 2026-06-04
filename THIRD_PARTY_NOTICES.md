# Third-party Notices

이 저장소는 다음 외부 라이브러리를 사용합니다.

## SFML 3.x (build-time dependency)

- 라이선스: zlib/libpng license
- 출처: https://github.com/SFML/SFML
- 본 저장소는 SFML 소스 코드를 직접 포함하지 않습니다. `BUILD_SFML_*` 옵션으로 빌드를 활성화하면 CMake `FetchContent` 가 빌드 시점에 v3.1.0 을 받아옵니다 (또는 시스템에 설치된 SFML 3.x 를 우선 사용).

## ncurses / PDCurses (build-time dependency)

- ncurses (Linux/macOS): X11/MIT-style license. 시스템 패키지로 설치.
- PDCurses (Windows MSYS2): public domain. 시스템 패키지로 설치.
- 본 저장소는 라이브러리 소스 코드를 포함하지 않습니다.

## 코드

본 저장소의 과제 코드와 헤더는 강의자가 작성한 학습용 자산이며 MIT 라이선스로 배포됩니다 (`LICENSE` 참고). 빌드 스크립트·CMake 도구·문서도 동일 라이선스 적용.
