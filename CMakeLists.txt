cmake_minimum_required(VERSION 3.23)

project(
  engine
  VERSION 1.0
  DESCRIPTION "My first SDL3 Game"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_PREFIX_PATH "nasm")
set(
  CMAKE_RUNTIME_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}/$<CONFIGURATION>"
)

set(
  CMAKE_LIBRARY_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}/$<CONFIGURATION>"
)

find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
    message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/includes/*.hpp")
file(GLOB_RECURSE CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/implements/*.cpp")
add_executable(engine ${CPP_FILES} ${HEADER_FILES})

set(VENDOR_DIR "${PROJECT_SOURCE_DIR}/vendor")
target_include_directories(engine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/includes
        ${VENDOR_DIR}/SDL/include
        ${VENDOR_DIR}/SDL_ttf/include
)

if(APPLE)
  set_target_properties(engine PROPERTIES
    INSTALL_RPATH "@executable_path;@loader_path"
    BUILD_WITH_INSTALL_RPATH TRUE
    MACOSX_RPATH TRUE
  )
elseif(UNIX)
  set_target_properties(engine PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
endif()

set(SDLTTF_VENDORED ON)
set(SDLTTF_USE_HARFBUZZ OFF)

add_subdirectory(${VENDOR_DIR}/SDL)
add_subdirectory(${VENDOR_DIR}/SDL_ttf)

target_link_libraries(engine 
  PRIVATE
    SDL3::SDL3 
    SDL3_ttf::SDL3_ttf
)