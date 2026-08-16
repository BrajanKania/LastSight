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
find_package(imgui CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Python3 REQUIRED)

set(REFLECTION_INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(GENERATE_REFLECTION_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/reflection/generate_reflection.py")
set(GENERATED_REFLECTION_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/engine/reflection/generated_reflection.cpp")

file(GLOB_RECURSE ALL_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_custom_command(
  OUTPUT "${GENERATED_REFLECTION_FILE}"
  COMMAND Python3::Interpreter "${GENERATE_REFLECTION_SCRIPT}"
    --input-dir ${REFLECTION_INPUT_DIR}
    --output-file "${GENERATED_REFLECTION_FILE}"
  DEPENDS "${GENERATE_REFLECTION_SCRIPT}" ${ALL_HEADERS}
  VERBATIM
)

add_executable(LastSight
  src/main.cpp

  # Engine ---

  src/engine/core/editor_layer.cpp
  src/engine/core/engine.cpp
  src/engine/core/random_system.cpp
  src/engine/core/scene_manager.cpp
  src/engine/core/stb_impl.cpp
  src/engine/core/window.cpp

  src/engine/gfx/framebuffer.cpp
  src/engine/gfx/shader.cpp
  src/engine/gfx/shader_exception.cpp
  src/engine/gfx/texture_2d.cpp
  src/engine/gfx/texture_manager.cpp

  src/engine/input/input_manager.cpp
  src/engine/input/input_system.cpp

  src/engine/interactions/interaction_system.cpp

  src/engine/particles/particle_system.cpp

  src/engine/physics/physics_system.cpp

  src/engine/renderer/passes/fov_pass.cpp
  src/engine/renderer/passes/lit_pass.cpp
  src/engine/renderer/passes/post_process_pass.cpp

  src/engine/renderer/render_system.cpp

  src/engine/serialization/scene_serializer.cpp
  src/engine/serialization/serialization_system.cpp

  src/engine/systems/parallax_system.cpp

  src/engine/ui/panels/asset_browser.cpp
  src/engine/ui/panels/entity_inspector_panel.cpp
  src/engine/ui/panels/main_menu_bar_panel.cpp
  src/engine/ui/panels/render_pipeline_debug_panel.cpp
  src/engine/ui/panels/scene_browser_panel.cpp
  src/engine/ui/panels/scene_hierarchy_panel.cpp
  src/engine/ui/panels/viewport_panel.cpp

  src/engine/ui/ui_manager.cpp
  src/engine/ui/ui_system.cpp

  # Game ---

  src/game/factories/projectile_factory.cpp


  src/game/scenes/main_menu_scene.cpp
  src/game/scenes/world_scene.cpp

  src/game/systems/camera_system.cpp
  src/game/systems/combat_system.cpp
  src/game/systems/equip_system.cpp
  src/game/systems/fov_system.cpp
  src/game/systems/inventory_system.cpp
  src/game/systems/lamp_system.cpp
  src/game/systems/player_system.cpp
  src/game/systems/post_process_system.cpp
  src/game/systems/projectile_system.cpp

  src/game/ui/inventory_panel.cpp
  src/game/ui/main_menu_panel.cpp

  # Generated ---

  ${GENERATED_REFLECTION_FILE}
)

target_link_libraries(
  LastSight
  PRIVATE
    glad2_lib
    glm::glm
    OpenGL::OpenGL
    SDL3::SDL3
    imgui::imgui
    EnTT::EnTT
    nlohmann_json::nlohmann_json
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

