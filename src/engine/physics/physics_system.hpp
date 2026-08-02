#pragma once

#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"

namespace ls::physics_system {

  void update(ecs::Registry& registry, dispatch::EventQueue& eventQueue, float dt);

}
