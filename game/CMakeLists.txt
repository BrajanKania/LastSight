project(LastSightGame LANGUAGES C CXX)

add_executable(LastSight)

target_sources(LastSight
  PRIVATE
    src/extraction/extraction_system.cpp

    src/factories/projectile_factory.cpp

    src/renderer/passes/lit_pass.cpp
    src/renderer/passes/fov_pass.cpp
    src/renderer/passes/post_process_pass.cpp

    src/scenes/main_menu_scene.cpp
    src/scenes/world_scene.cpp

    src/systems/camera_system.cpp
    src/systems/combat_system.cpp
    src/systems/equip_system.cpp
    src/systems/fov_system.cpp
    src/systems/inventory_system.cpp
    src/systems/lamp_system.cpp
    src/systems/player_system.cpp
    src/systems/post_process_system.cpp
    src/systems/projectile_system.cpp

    src/ui/inventory_panel.cpp
    src/ui/main_menu_panel.cpp

    # Main ---
    src/main.cpp
)

target_include_directories(LastSight
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(LastSight
    PRIVATE
        Engine
)
