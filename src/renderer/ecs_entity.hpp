#pragma once

#include <cstdint>

namespace ecs {
class Entity {
 public:
  using id_t = uint32_t;       // 24 bits
  using genid_t = uint8_t;     // 8 bits
  using valueid_t = uint32_t;  // 32 bits

  static const uint32_t GEN_MASK = 0xFF000000;
  static const uint32_t ID_MASK = 0x00FFFFFF;

  Entity() : valueid(0) {}

  Entity(id_t id, genid_t gen) : valueid{(static_cast<valueid_t>(gen) << 24) | (id & ID_MASK)} {}

  id_t getId() const { return valueid & ID_MASK; }

  genid_t getGen() const { return valueid >> 24; }

  void setId(id_t id) { valueid = (valueid & GEN_MASK) | (id & ID_MASK); }

  void setGen(genid_t gen) { valueid = (static_cast<valueid_t>(gen) << 24) | (valueid & ID_MASK); }

 private:
  valueid_t valueid = 0;
  // bool operator==(const RLMEntity &other) const { return id == other.id; }
};
}  // namespace ecs
