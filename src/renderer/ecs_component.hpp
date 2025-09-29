#pragma once

#include "renderer/ecs_id_generator.hpp"
#include <cstdint>

namespace ecs {

static inline IDGenerator componentIDGenerator;

template <typename C> static uint32_t getComponentID() {
  // C++ guarantees that this line is executed only once, safely,
  // even if the IDGenerator itself is non-atomic.
  static const uint32_t componentId = componentIDGenerator.nextID();
  return componentId;
}

}  // namespace ecs
