#pragma once

#include <cstdint>

namespace ecs {
using EntityID = uint32_t;

class Entity {
 public:
  using physid_t = uint32_t;  // 24 bits physical id
  using genid_t = uint8_t;    // 8 bits generational id

  static const uint32_t GEN_MASK = 0xFF000000;
  static const uint32_t ID_MASK = 0x00FFFFFF;

  static EntityID createEntity(physid_t id = 0, genid_t gen = 0) {
    return (static_cast<EntityID>(gen) << 24 | (id & ID_MASK));
  }

  Entity() = delete;
  ~Entity() = delete;
  Entity &operator=(const Entity &) = delete;
  Entity &operator=(Entity &&) = delete;

  static physid_t getId(EntityID entityId) { return entityId & ID_MASK; }

  static genid_t getGen(EntityID entityId) { return entityId >> 24; }

  static void setId(EntityID &entityId, physid_t id) { entityId = (entityId & GEN_MASK) | (id & ID_MASK); }

  static void setGen(EntityID &entityId, genid_t gen) {
    entityId = (static_cast<EntityID>(gen) << 24) | (entityId & ID_MASK);
  }
};
}  // namespace ecs
