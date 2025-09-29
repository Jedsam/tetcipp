#pragma once

#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ecs_entity.hpp"

namespace ecs {

struct Register {
 public:
  using Column = void *;
  using ComponentId = uint32_t;
  using ArchetypeId = uint32_t;
  using Type = std::vector<ComponentId>;

  struct Archetype {
    // the lookup id
    ArchetypeId id;
    // this should be sorted
    Type type;
    // void pointer to be cast to the right value later on, it is the same
    // order as the type variable
    std::vector<Column> components;
  };

  struct Record {
    Archetype &archetype;
    size_t row;
  };

  EntityID createEntity();
  void deleteEntity(EntityID entity);
  bool isEntityAlive(EntityID entity);

  template <typename Component> void addComponent(Component component, EntityID entity) {
    Record record = entityIndex[entity];
  }

  template <typename Component> void updateComponent(Component component, EntityID entity) {}

  template <typename Component> void deleteComponent(EntityID entity);

  Archetype &findArchetype(EntityID entity) { return entityIndex[entity].archetype; }

  // For performance reasons its better to put the component likely to have the least amount of members as
  // first parameter
  template <typename C, typename... Components> std::vector<Archetype> findComponents() {
    ComponentId componentId = getComponentId<C>();
    auto &resultSet = componentIndex[componentId];

    ((resultSet = intersect(resultSet, componentIndex[getComponentId<Components>()])), ...);

    std::vector<Archetype> result{};

    std::transform(resultSet.begin(), resultSet.end(), result.begin(), [this](auto archetypeId) {
      return archetypeIndex[archetypeId];
    });

    return result;
  }

  using ArchetypeSet = std::unordered_set<ArchetypeId>;

  ArchetypeSet static insersect(ArchetypeSet &set1, ArchetypeSet &set2) {
    ArchetypeSet resultSet{};
    for (auto setElement : set1) {
      if (set2.contains(setElement)) {
        resultSet.insert(setElement);
      }
    }
    return resultSet;
  }

 private:
  EntityID nextId = 1;

  std::unordered_map<EntityID, Record> entityIndex;
  std::unordered_map<ArchetypeId, Archetype> archetypeIndex;

  // Find the archetypes for a component
  std::unordered_map<ComponentId, ArchetypeSet> componentIndex;
};
}  // namespace ecs
