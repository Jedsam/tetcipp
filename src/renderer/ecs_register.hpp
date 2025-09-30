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
      return static_cast<char *>(data) + index * element_size;
    }

    template <typename Component> Component *at(size_t index) {
      if (index >= count) {
        return nullptr;
      }
      return &(reinterpret_cast<Component *>(data))[index];
    }

    template <typename Component>
    void pushBack(const Component &component, size_t index) {
      if (count > capacity / 2) {
        data = reallocarray(data, capacity * 2, element_size);
        capacity *= 2;
      }
      std::memcpy(component, at(count), element_size);
      count++;
    }

    void pushBack(Column &inputColumn, size_t index) {
      if (count > capacity / 2) {
        data = reallocarray(data, capacity * 2, element_size);
        capacity *= 2;
      }
      std::memcpy(inputColumn.at(index), at(count), element_size);
      count++;
    }

    void deleteElement(size_t index) {
      // replace the element with the top value
      std::memcpy(at(count - 1), at(index), element_size);
      if (count < capacity / 2) {
        data = reallocarray(data, capacity / 2, element_size);
        capacity /= 2;
      }
      count--;
    }

   private:
    void *data = nullptr;
    size_t element_size = 0;

    size_t count = 0;
    size_t capacity = 16;
  };  // namespace ecs

  using ComponentId = uint32_t;
  using ArchetypeId = uint32_t;

  // Basically a sorted vector of component ids
  struct Type {
   public:
    auto begin() { return componentIds.begin(); }

    auto begin() const { return componentIds.begin(); }

    auto end() { return componentIds.end(); }

    auto end() const { return componentIds.end(); }

    Type clone() {
      Type clonedType;
      clonedType.componentIds = componentIds;
      return clonedType;
    }

    // adds an element to the sorted list using binary searc
    void add(ComponentId id) {
      auto it = std::lower_bound(componentIds.begin(), componentIds.end(), id);

      if (it == componentIds.end() || *it != id) {
        componentIds.insert(it, id);
      }
    }

    ComponentId &operator[](size_t index) { return componentIds[index]; }

   private:
    std::vector<ComponentId> componentIds;
  };

  struct Archetype;

  struct ArchetypeEdge {
    Archetype &edge;  // both add and remove are same
  };

  struct Archetype {
   public:
    // this should be sorted
    Type type;
    // void pointer to be cast to the right value later on, it is the same
    // order as the type variable
    std::vector<Column> components;
    std::unordered_map<ComponentId, ArchetypeEdge> edges;

    // Adds a value to the archetype given the archetype the components resides
    // in, row value of the entity components and the new component to add
    template <typename Component>
    void addValue(Archetype oldArchetype, Component component, size_t row) {
      auto &newComponents = components;
      auto &newType = type;
      auto &oldComponents = oldArchetype.components;
      auto &oldType = oldArchetype.type;
      for (int i = 0, j = 0; i < newComponents.size(); i++, j++) {
        if (newType[i] == oldType[i]) {
          newComponents[i].pushBack(oldComponents[j], row);
        } else {
          newComponents[i].pushBack<Component>(component, row);
          j--;
        }
      }
    }
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
    ComponentId componentId = getComponentId<Component>();
    auto it = edgesMap.find(componentId);
    Archetype &oldArchetype = record.archetype;
    if (it != edgesMap.end()) {
      Archetype &newArchetype = it->second.edge;
    } else {
      Type newType = oldArchetype.type.clone();
      newType.add(getComponentId<Component>());
      auto itArche = archetypeIndex.find(newType);
      // if it doesnt exist create a new archetype
      if (itArche == archetypeIndex.end()) {
        Archetype newArchetype;
        newArchetype.type = newType;
        newArchetype.components = newType.initVector();
        newArchetype.edges[componentId] = oldArchetype;
      }
    }

    newArchetype.addValue<Component>(record.archetype, component, record.row);

    // update the EntityIndex map
    entityIndex[entity].archetype = newArchetype;
    entityIndex[entity].row = newArchetype.components[0].size();
    // place it inside
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
  std::unordered_map<Type, Archetype, TypeHasher> archetypeIndex;

  // Find the archetypes for a component
  std::unordered_map<ComponentId, ArchetypeSet> componentIndex;
};
}  // namespace ecs
