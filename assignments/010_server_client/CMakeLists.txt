cmake_minimum_required(VERSION 3.20)
project(tetris_assignment_010_server_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SFML_NETWORK_CLIENT "Build server-connected SFML Network bot client" OFF)

set(TETRIS010_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/support")

add_library(tetris010_client
    ${TETRIS010_SUPPORT_DIR}/007_protocol/network/Protocol.cpp
    ${TETRIS010_SUPPORT_DIR}/008_transport/network/InMemoryTransport.cpp
    ${TETRIS010_SUPPORT_DIR}/008_transport/client/NetworkClient.cpp
    ${TETRIS010_SUPPORT_DIR}/009_tcp_transport/network/LineFramer.cpp
    client/NetworkGameApp.cpp
    client/RemoteRenderMapper.cpp
)
target_include_directories(tetris010_client PUBLIC
    ${TETRIS010_SUPPORT_DIR}/007_protocol/core
    ${TETRIS010_SUPPORT_DIR}/007_protocol/network
    ${TETRIS010_SUPPORT_DIR}/008_transport/network
    ${TETRIS010_SUPPORT_DIR}/008_transport/client
    ${TETRIS010_SUPPORT_DIR}/005_ports_ncurses/app
    ${TETRIS010_SUPPORT_DIR}/005_ports_ncurses/ports
    ${TETRIS010_SUPPORT_DIR}/004_full_rules
    ${TETRIS010_SUPPORT_DIR}/009_tcp_transport/network
    ${CMAKE_CURRENT_SOURCE_DIR}/client
)

if(BUILD_ASSIGNMENT_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_server_client.cpp")
    enable_testing()
    add_executable(test_assignment_010 tests/test_server_client.cpp)
    target_link_libraries(test_assignment_010 PRIVATE tetris010_client)
    add_test(NAME assignment_010_client_logic COMMAND test_assignment_010)

    add_executable(case_runner_010 tests/case_runner.cpp)
    target_link_libraries(case_runner_010 PRIVATE tetris010_client)
    add_test(
        NAME assignment_010_client_case
        COMMAND ${CMAKE_COMMAND}
            -DCASE_RUNNER=$<TARGET_FILE:case_runner_010>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/tests/data/server_client_cases.in
            -DEXPECTED_FILE=${CMAKE_CURRENT_SOURCE_DIR}/tests/data/server_client_cases.out
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_case.cmake
    )
endif()

if(BUILD_SFML_NETWORK_CLIENT)
    include(${CMAKE_SOURCE_DIR}/cmake/SfmlSetup.cmake)

    add_library(tetris010_sfml_tcp network/SfmlTcpTransport.cpp)
    target_include_directories(tetris010_sfml_tcp PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/network
        ${TETRIS010_SUPPORT_DIR}/008_transport/network
        ${TETRIS010_SUPPORT_DIR}/009_tcp_transport/network
    )
    target_link_libraries(tetris010_sfml_tcp PUBLIC tetris010_client SFML::Network)

    add_executable(tetris_client_bot client/main_client_bot.cpp)
    target_link_libraries(tetris_client_bot PRIVATE tetris010_sfml_tcp)

    add_library(tetris010_sfml_adapters
        ${TETRIS010_SUPPORT_DIR}/006_sfml_infra/adapters/sfml/SfmlInput.cpp
        ${TETRIS010_SUPPORT_DIR}/006_sfml_infra/adapters/sfml/SfmlRenderer.cpp
    )
    target_include_directories(tetris010_sfml_adapters PUBLIC
        ${TETRIS010_SUPPORT_DIR}/006_sfml_infra/adapters/sfml
        ${TETRIS010_SUPPORT_DIR}/005_ports_ncurses/app
        ${TETRIS010_SUPPORT_DIR}/005_ports_ncurses/ports
    )
    target_link_libraries(tetris010_sfml_adapters PUBLIC tetris010_client SFML::Graphics SFML::Window)

    add_executable(tetris010_sfml_client client/main_sfml_client.cpp)
    target_link_libraries(tetris010_sfml_client PRIVATE tetris010_sfml_tcp tetris010_sfml_adapters)
endif()
