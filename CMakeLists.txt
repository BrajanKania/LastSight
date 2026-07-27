cmake_minimum_required(VERSION 3.23)

project(LastSight LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
  glad2
  GIT_REPOSITORY https://github.com/Dav1dde/glad
  GIT_TAG e3c14f906127dea49ead20ff4d5878294d08b7ab
  SOURCE_SUBDIR cmake
  GIT_SHALLOW ON
)
FetchContent_MakeAvailable(glad2)
glad_add_library(glad2_lib STATIC API gl:core=4.6)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(SDL3 REQUIRED COMPONENTS SDL3-static)
find_package(glm REQUIRED)

add_subdirectory(src/renderer/shader)

add_executable(LastSight 
  src/main.cpp

  src/core/window.cpp
  src/core/controller_system.cpp
  src/core/scene_manager.cpp

  src/renderer/renderer_system.cpp

  src/scene/world_scene.cpp
)

target_link_libraries(
  LastSight
  PRIVATE
    shader
    OpenGL::OpenGL
    SDL3::SDL3
)

add_custom_command(
  TARGET LastSight POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/assets"
  "${CMAKE_CURRENT_BINARY_DIR}/assets"
)

