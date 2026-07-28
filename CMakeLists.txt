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

  src/controller/controller_system.cpp
  
  src/renderer/renderer_system.cpp
  src/renderer/render_passes/lit_pass.cpp
  src/renderer/render_passes/compose_pass.cpp
  src/renderer/render_passes/post_process_pass.cpp
  src/renderer/framebuffer.cpp

  src/scene/scene_manager.cpp
  src/scene/scenes/world_scene.cpp

  src/window/window.cpp
)

target_link_libraries(
  LastSight
  PRIVATE
    glad2_lib
    glm::glm
    OpenGL::OpenGL
    SDL3::SDL3
    shader
)

target_include_directories(
  LastSight 
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

option(USE_LINK_TO_ASSETS_DIR "" OFF)

if(USE_LINK_TO_ASSETS_DIR)
  if(EXISTS "${CMAKE_BINARY_DIR}/assets" AND NOT IS_SYMLINK "${CMAKE_BINARY_DIR}/assets")
      file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/assets")
  endif()

  if(NOT EXISTS "${CMAKE_BINARY_DIR}/assets")
      file(CREATE_LINK
          "${CMAKE_CURRENT_SOURCE_DIR}/assets"
          "${CMAKE_BINARY_DIR}/assets"
          SYMBOLIC
      )
      message(STATUS "Created symlink to assets: ${CMAKE_BINARY_DIR}/assets")
  endif()
else()
  add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "${CMAKE_BINARY_DIR}/assets"
  )
  add_dependencies(LastSight copy_assets)
endif()

