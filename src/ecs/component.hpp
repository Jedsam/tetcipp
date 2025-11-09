#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

namespace ecs {
using ComponentID = uint32_t;
using ArchetypeId = uint32_t;

class ComponentIDGenerator {
 public:
  template <typename Component> static uint32_t nextID() {
    componentSizes.push_back(sizeof(Component));
    return current_id.fetch_add(1);
  }

  template <typename Component> static uint32_t getComponentID() {
    // C++ guarantees that this line is executed only once, safely,
    // even if the IDGenerator itself is non-atomic.
    static const uint32_t componentID = nextID<Component>();
    return componentID;
  }

  static size_t getComponentSize(ComponentID componentID) {
    return componentSizes[componentID];
  }

 private:
  inline static std::atomic<ComponentID> current_id = 1;
  inline static std::vector<ComponentID> componentSizes{0};
};

}  // namespace ecs
