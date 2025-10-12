#pragma once

#include "renderer/ecs_id_generator.hpp"
#include <cstdint>

namespace ecs {

class IDGenerator {
 public:
  uint32_t nextID() { return current_id.fetch_add(1); }

 private:
  std::atomic<uint32_t> current_id = 1;
  std::vector<ComponentId> componentSizes;
};

template <typename C> static uint32_t getComponentID() {
  // C++ guarantees that this line is executed only once, safely,
  // even if the IDGenerator itself is non-atomic.
  static const uint32_t componentId = componentIDGenerator.nextID();
  return componentId;
}

}  // namespace ecs
