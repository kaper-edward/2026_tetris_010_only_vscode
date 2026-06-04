# SfmlSetup.cmake — locate or fetch SFML 3.x for optional local clients.
#
# Renderer-only builds need Graphics/Window. Network is requested when the
# optional assignment 010 network client is enabled.

if(_TETRIS_SFML_SETUP_DONE)
    return()
endif()
set(_TETRIS_SFML_SETUP_DONE TRUE)

set(_TETRIS_SFML_NEEDS_GRAPHICS OFF)
foreach(option_name IN ITEMS BUILD_SFML_RENDERER BUILD_SFML_NETWORK_CLIENT)
    if(DEFINED ${option_name} AND ${option_name})
        set(_TETRIS_SFML_NEEDS_GRAPHICS ON)
    endif()
endforeach()

set(_TETRIS_SFML_NEEDS_NETWORK OFF)
foreach(option_name IN ITEMS BUILD_SFML_NETWORK_CLIENT)
    if(DEFINED ${option_name} AND ${option_name})
        set(_TETRIS_SFML_NEEDS_NETWORK ON)
    endif()
endforeach()

set(_TETRIS_SFML_COMPONENTS System)
if(_TETRIS_SFML_NEEDS_GRAPHICS)
    list(APPEND _TETRIS_SFML_COMPONENTS Window Graphics)
endif()
if(_TETRIS_SFML_NEEDS_NETWORK)
    list(APPEND _TETRIS_SFML_COMPONENTS Network)
endif()
find_package(SFML 3 CONFIG QUIET COMPONENTS ${_TETRIS_SFML_COMPONENTS})

if(SFML_FOUND)
    message(STATUS "SFML: using system installation (version ${SFML_VERSION})")
else()
    message(STATUS "SFML: system not found, fetching v3.1.0 from GitHub")
    if(_TETRIS_SFML_NEEDS_NETWORK)
        message(STATUS "SFML: Network module requires MbedTLS/Libssh2 development files (Ubuntu: libmbedtls-dev libssh2-1-dev)")
    endif()
    include(FetchContent)
    FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
    )
    set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_GRAPHICS ${_TETRIS_SFML_NEEDS_GRAPHICS} CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK ${_TETRIS_SFML_NEEDS_NETWORK} CACHE BOOL "" FORCE)
    set(SFML_BUILD_WINDOW ${_TETRIS_SFML_NEEDS_GRAPHICS} CACHE BOOL "" FORCE)
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(sfml)
endif()
