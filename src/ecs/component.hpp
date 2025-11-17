#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

namespace ecs {
using ComponentID = uint32_t;
using ArchetypeId = uint32_t;

class ComponentIDGenerator {
 public:
  struct ComponentInfo {
    size_t size;
    size_t align;
    void (*ctor)(void *, int);
    void (*dtor)(void *, int);
    void (*move)(void *, void *, int);
  };

  template <typename Component>
  static void ctor_function(void *ptr, int count) {
    std::uninitialized_value_construct_n(
        reinterpret_cast<Component *>(ptr), count);
  }

  template <typename Component>
  static void dtor_function(void *ptr, int count) {
    std::destroy_n(reinterpret_cast<Component *>(ptr), count);
  }

  template <typename Component>
  static void move_function(void *dst, void *src, int count) {
    std::uninitialized_move_n(
        reinterpret_cast<Component *>(src),
        count,
        reinterpret_cast<Component *>(dst));
  }

  template <typename Component> static uint32_t nextID() {
    return current_id.fetch_add(1);
  }

  template <typename Component> static void registerComponent() {
    ComponentInfo ti;
    ti.size = sizeof(Component);
    ti.align = alignof(Component);

    ti.ctor = &ctor_function<Component>;
    ti.dtor = &dtor_function<Component>;
    ti.move = &move_function<Component>;

    uint32_t id = getComponentID<Component>();
    if (id >= typeInfoMap.size()) {
      typeInfoMap.resize(id + 1);
    }

    typeInfoMap[getComponentID<Component>()] = ti;
  }

  template <typename Component> static uint32_t getComponentID() {
    // C++ guarantees that this line is executed only once, safely,
    // even if the IDGenerator itself is non-atomic.
    static const uint32_t componentID = nextID<Component>();
    return componentID;
  }

  static size_t getComponentSize(ComponentID componentID) {
    return typeInfoMap[componentID].size;
  }

  inline static std::vector<ComponentInfo> typeInfoMap{ComponentInfo{}};

 private:
  inline static std::atomic<ComponentID> current_id = 1;
};

}  // namespace ecs
