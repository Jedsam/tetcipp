#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ecs_entity.hpp"

namespace ecs {

struct Register {
 public:
  struct Column {
   public:
    size_t size() { return count; }

    void *at(size_t index) {
      if (index >= count) {
        return nullptr;
      }
      return data + index * element_size;
    }

    template <typename Component> Component *at(size_t index) {
      if (index >= count) {
        return nullptr;
      }
      return &(reinterpret_cast<Component *>(data))[index];
    }

    size_t pushBack(Column &inputColumn, size_t index) {
      if (count < capacity / 2) {
        data = reallocarray(data, capacity * 2, element_size);
      }
      memccpy(, const void *__restrict src, int c, size_t n) return -1;
    }

   private:
    void *data;
    size_t element_size;

    size_t count = 0;
    size_t capacity = 10;
  };  // namespace ecs

  using ComponentId = uint32_t;
  using ArchetypeId = uint32_t;
  using Type = std::vector<ComponentId>;

  struct Archetype;

  struct ArchetypeEdge {
    Archetype &add;
    Archetype &remove;
  };

  struct Archetype {
    // this should be sorted
    Type type;
    // void pointer to be cast to the right value later on, it is the same
    // order as the type variable
    Column components;
    std::unordered_map<ComponentId, ArchetypeEdge> edges;
  };

  struct Record {
    Archetype &archetype;
    size_t row;
  };

  EntityID createEntity();
  void deleteEntity(EntityID entity);
  bool isEntityAlive(EntityID entity);

  template <typename Component>
  void addComponent(Component component, EntityID entity) {
    Record record = entityIndex[entity];
    auto edgesMap = record.archetype.edges;
    auto it = edgesMap.find(getComponentId<Component>());

    if (it != edgesMap.end()) {
      Archetype &newArchetype = it->add;
      Column &newComponents = newArchetype.components;
      Column &oldComponents = record.archetype.components;
      int i = 0;
      for (; i < oldComponents.size(); i++) {
        newComponents[i].pushBack(oldComponents, record.row);
      }
      newComponents[i].pushBackM<Component>(component, record.row);

      // Add the new component
      entityIndex[entity].archetype = result;
    } else {
    }
  }

  template <typename Component>
  void updateComponent(Component component, EntityID entity) {}

  template <typename Component> void deleteComponent(EntityID entity);

  Archetype &findArchetype(EntityID entity) {
    return entityIndex[entity].archetype;
  }

  using ArchetypeSet = std::unordered_set<Archetype *>;

  static ArchetypeSet
  intersect(const ArchetypeSet &set1, const ArchetypeSet &set2) {
    ArchetypeSet resultSet{};
    for (auto setElement : set1) {
      if (set2.contains(setElement)) {
        resultSet.insert(setElement);
      }
    }
    return resultSet;
  }

  // For performance reasons its better to put the component likely to have the
  // least amount of members as first parameter
  template <typename C, typename... Components> ArchetypeSet findComponents() {
    ComponentId componentId = getComponentId<C>();
    ArchetypeSet resultSet = componentIndex[componentId];

    ((resultSet =
          intersect(resultSet, componentIndex[getComponentId<Components>()])),
     ...);

    return resultSet;
  }

 private:
  EntityID nextId = 1;

  std::unordered_map<EntityID, Record> entityIndex;

  struct TypeHasher {
    size_t operator()(const Type &type) const {
      // Combine hashes using the bitwise XOR
      return std::accumulate(
          type.begin(),
          type.end(),
          0,
          [](ComponentId component1, ComponentId component2) {
            return component1 ^ component2;
          });
    }
  };

  std::vector<uint32_t> componentSize;
  // Types not being sorted might lead to bugs later on
  std::unordered_map<Type, Archetype, TypeHasher> archetypeIndex;

  // Find the archetypes for a component
  std::unordered_map<ComponentId, ArchetypeSet> componentIndex;
};
}  // namespace ecs
