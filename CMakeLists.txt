cmake_minimum_required(VERSION 3.20)
project(tetris_010_vscode LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MINGW)
    link_libraries(stdc++exp)
endif()

option(BUILD_ASSIGNMENT_TESTS "Build private assignment logic tests when present" OFF)

set(BUILD_SFML_NETWORK_CLIENT ON CACHE BOOL "Build assignment 010 SFML Network clients" FORCE)

add_subdirectory(assignments/010_server_client)
