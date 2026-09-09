project(LastSightEngine LANGUAGES C CXX)

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
find_package(SDL3 CONFIG REQUIRED)
find_package(glm REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Python3 REQUIRED)

set(REFLECTION_INPUT_DIRS
  "${CMAKE_SOURCE_DIR}/engine/src"
  "${CMAKE_SOURCE_DIR}/game/src"
)

set(REFLECTION_GENERATOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/ls/reflection/generator")
set(GENERATE_REFLECTION_SCRIPT "${REFLECTION_GENERATOR_DIR}/main.py")
set(GENERATED_REFLECTION_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/engine/reflection/generated_reflection.cpp")

file(GLOB_RECURSE ALL_HEADERS
    "${CMAKE_SOURCE_DIR}/engine/src/*.hpp"
    "${CMAKE_SOURCE_DIR}/game/src/*.hpp"
)

add_custom_command(
  OUTPUT "${GENERATED_REFLECTION_FILE}"
  COMMAND Python3::Interpreter "${GENERATE_REFLECTION_SCRIPT}"
    --dirs ${REFLECTION_INPUT_DIRS}
    --output-file "${GENERATED_REFLECTION_FILE}"
  DEPENDS "${GENERATE_REFLECTION_SCRIPT}" ${ALL_HEADERS}
  VERBATIM
)

add_library(Engine STATIC)

target_compile_features(Engine
  PUBLIC
    cxx_std_23
)

set_target_properties(Engine
  PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_sources(Engine
  PRIVATE
    src/ls/core/editor_layer.cpp
    src/ls/core/engine.cpp
    src/ls/core/random_system.cpp
    src/ls/core/scene_manager.cpp
    src/ls/core/stb_impl.cpp
    src/ls/core/window.cpp

    src/ls/debug/console.cpp
    src/ls/debug/status_bar.cpp

    src/ls/extraction/extraction_system.cpp

    src/ls/gfx/framebuffer.cpp
    src/ls/gfx/shader.cpp
    src/ls/gfx/shader_exception.cpp
    src/ls/gfx/texture_2d.cpp
    src/ls/gfx/texture_manager.cpp

    src/ls/input/input_manager.cpp
    src/ls/input/input_system.cpp

    src/ls/interactions/interaction_system.cpp

    src/ls/particles/particle_system.cpp

    src/ls/physics/physics_system.cpp

    src/ls/platform/external_tool_manager.cpp

    src/ls/prefab/prefab_manager.cpp

    src/ls/renderer/passes/default_lit_pass.cpp

    src/ls/renderer/material/material.cpp
    src/ls/renderer/material/material_manager.cpp

    src/ls/renderer/render_pipeline.cpp
    src/ls/renderer/render_system.cpp

    src/ls/serialization/prefab_serializer.cpp
    src/ls/serialization/scene_serializer.cpp
    src/ls/serialization/serialization_system.cpp

    src/ls/systems/parallax_system.cpp

    src/ls/ui/panels/asset_browser_panel.cpp
    src/ls/ui/panels/console_panel.cpp
    src/ls/ui/panels/entity_inspector_panel.cpp
    src/ls/ui/panels/main_menu_bar_panel.cpp
    src/ls/ui/panels/render_pipeline_debug_panel.cpp
    src/ls/ui/panels/scene_browser_panel.cpp
    src/ls/ui/panels/scene_hierarchy_panel.cpp
    src/ls/ui/panels/status_bar_panel.cpp
    src/ls/ui/panels/viewport_panel.cpp

    src/ls/ui/ui_manager.cpp
    src/ls/ui/ui_system.cpp

    ${GENERATED_REFLECTION_FILE}
)

target_include_directories(Engine
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
  PRIVATE
    "${CMAKE_SOURCE_DIR}/game/src"
)

target_link_libraries(Engine
  PUBLIC
    glm::glm
    EnTT::EnTT
    imgui::imgui
    nlohmann_json::nlohmann_json
    glad2_lib
    OpenGL::GL
    SDL3::SDL3
)

option(USE_LINK_TO_ASSETS_DIR "" OFF)

if(USE_LINK_TO_ASSETS_DIR)
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/game")

  if(EXISTS "${CMAKE_BINARY_DIR}/game/assets" AND NOT IS_SYMLINK "${CMAKE_BINARY_DIR}/game/assets")
    file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/game/assets")
  endif()

  if(NOT EXISTS "${CMAKE_BINARY_DIR}/game/assets")
    file(CREATE_LINK
            "${CMAKE_SOURCE_DIR}/game/assets"
            "${CMAKE_BINARY_DIR}/game/assets"
            SYMBOLIC
        )
  endif()

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/engine")

  if(EXISTS "${CMAKE_BINARY_DIR}/engine/assets" AND NOT IS_SYMLINK "${CMAKE_BINARY_DIR}/engine/assets")
    file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/engine/assets")
  endif()

  if(NOT EXISTS "${CMAKE_BINARY_DIR}/engine/assets")
    file(CREATE_LINK
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "${CMAKE_BINARY_DIR}/engine/assets"
            SYMBOLIC
        )
  endif()
else()
  add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/game/assets"
            "${CMAKE_BINARY_DIR}/game/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "${CMAKE_BINARY_DIR}/engine/assets"
    )
  add_dependencies(Engine copy_assets)
endif()

