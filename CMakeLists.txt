cmake_minimum_required(VERSION 3.23)

project(LastSightProject LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

file(CREATE_LINK
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${CMAKE_SOURCE_DIR}/compile_commands.json"
    SYMBOLIC COPY_ON_ERROR
)

add_subdirectory(engine "${CMAKE_BINARY_DIR}/engine_build")
add_subdirectory(game "${CMAKE_BINARY_DIR}/game_build")
