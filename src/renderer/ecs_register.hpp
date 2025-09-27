#pragma once

#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ecs_entity.hpp"

namespace ecs {

class Register {
 public:
  Entity createEntity();
  void deleteEntity(Entity entity);
  bool isEntityAlive(Entity entity);

  using ComponentId = uint32_t;
  using ArchetypeId = uint32_t;
  using Type = std::vector<ComponentId>;

  struct Archetype {
    ArchetypeId id;
    Type type;  // this should be sorted
  };

 private:
  Entity::physid_t nextId = 1;

  std::unordered_map<EntityID, Archetype &> entityIndex;
  std::unordered_map<Type, Archetype> archetypeIndex;

  // Find the archetypes for a component
  using ArchetypeSet = std::unordered_set<ArchetypeId>;
  std::unordered_map<ComponentId, ArchetypeSet> component_index;
};
}  // namespace ecs
