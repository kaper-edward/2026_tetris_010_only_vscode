cmake_minimum_required(VERSION 3.20)
project(tetris_assignment_011_network_versus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The ncurses client only builds where curses development headers exist
# (Linux/macOS). Enable with: cmake --preset default -DBUILD_NCURSES_VERSUS_CLIENT=ON
option(BUILD_NCURSES_VERSUS_CLIENT "Build the ncurses dual-board versus client" OFF)

# View-model layer: maps server VERSUS_SNAPSHOT data to a screen-level
# view (VersusViewSnapshot). Reuses protocol/transport from tetris010_client.
add_library(tetris011_view
    core/TetrominoPreview.cpp
    client/VersusViewSnapshot.cpp
    client/VersusViewMapper.cpp
)
target_include_directories(tetris011_view PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/client
)
target_link_libraries(tetris011_view PUBLIC tetris010_client)

if(BUILD_SFML_NETWORK_CLIENT)
    include(${CMAKE_SOURCE_DIR}/cmake/SfmlSetup.cmake)

    add_library(tetris011_sfml_renderer adapters/sfml/SfmlVersusRenderer.cpp)
    target_include_directories(tetris011_sfml_renderer PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/adapters/sfml
    )
    target_link_libraries(tetris011_sfml_renderer PUBLIC tetris011_view SFML::Graphics)

    add_executable(tetris011_sfml_versus_client client/main_sfml_versus_client.cpp)
    target_link_libraries(tetris011_sfml_versus_client PRIVATE tetris010_sfml_tcp tetris011_sfml_renderer)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
        add_custom_command(TARGET tetris011_sfml_versus_client POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/assets
                $<TARGET_FILE_DIR:tetris011_sfml_versus_client>/assets
        )
    endif()
endif()

if(BUILD_NCURSES_VERSUS_CLIENT)
    set(CURSES_NEED_NCURSES TRUE)
    find_package(Curses REQUIRED)
    add_library(tetris011_ncurses_renderer adapters/ncurses/NcursesVersusRenderer.cpp)
    target_include_directories(tetris011_ncurses_renderer PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/adapters/ncurses
        ${CURSES_INCLUDE_DIRS}
    )
    target_link_libraries(tetris011_ncurses_renderer PUBLIC tetris011_view ${CURSES_LIBRARIES})

    add_executable(tetris011_ncurses_versus_client client/main_ncurses_versus_client.cpp)
    target_link_libraries(tetris011_ncurses_versus_client PRIVATE tetris010_sfml_tcp tetris011_ncurses_renderer)
endif()
