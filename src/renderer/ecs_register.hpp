#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ecs_entity.hpp"

namespace ecs {

using ComponentID = uint32_t;
using ArchetypeId = uint32_t;

class ComponentIDGenerator {
 public:
  template <typename Component> static uint32_t nextID() {
    componentSizes[current_id] = sizeof(Component);
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
  static std::vector<ComponentID> componentSizes;
};

struct Register {
 public:
  struct Column {
   public:
    explicit Column(size_t element_size) {
      data = malloc(capacity * element_size);
    }

    ~Column() { free(data); }

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

    template <typename Component>
    void updateElement(const Component &component, size_t index) {
      if (count >= capacity) {
        return;
      }
      std::memcpy(component, at(count), element_size);
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

  // Basically a sorted vector of component ids
  struct Type {
   public:
    auto begin() { return componentIDs.begin(); }

    auto begin() const { return componentIDs.begin(); }

    auto end() { return componentIDs.end(); }

    auto end() const { return componentIDs.end(); }

    size_t find(ComponentID id) {
      auto it = std::lower_bound(componentIDs.begin(), componentIDs.end(), id);
      return std::distance(componentIDs.begin(), it);
    }

    Type clone() {
      Type clonedType;
      clonedType.componentIDs = componentIDs;
      return clonedType;
    }

    std::vector<Column> initComponentVector() {
      std::vector<Column> returnColumn;
      std::transform(
          componentIDs.begin(),
          componentIDs.end(),
          std::back_inserter(returnColumn),
          [](auto componentID) {
            return Column(ComponentIDGenerator::getComponentSize(componentID));
          });
      return returnColumn;
    }

    // adds an element to the sorted list using binary searc
    void add(ComponentID id) {
      auto it = std::lower_bound(componentIDs.begin(), componentIDs.end(), id);

      if (it == componentIDs.end() || *it != id) {
        componentIDs.insert(it, id);
      }
    }

    ComponentID &operator[](size_t index) { return componentIDs[index]; }

   private:
    std::vector<ComponentID> componentIDs;
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
    std::unordered_map<ComponentID, ArchetypeEdge> edges;

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
    ComponentID componentID = ComponentIDGenerator::getComponentID<Component>();
    auto it = edgesMap.find(componentID);
    Archetype &oldArchetype = record.archetype;
    Archetype *newArchetype;
    if (it != edgesMap.end()) {
      newArchetype = &it->second.edge;
    } else {
      Type newType = oldArchetype.type.clone();
      newType.add(ComponentIDGenerator::getComponentID<Component>());
      auto itArche = archetypeIndex.find(newType);
      // if it doesnt exist create a new archetype and insert it to the map
      if (itArche != archetypeIndex.end()) {
        newArchetype = &itArche->second;
      } else {
        Archetype newArchetype;
        newArchetype.type = newType;
        newArchetype.components = newType.initComponentVector();
        newArchetype.edges.emplace(componentID, oldArchetype);

        archetypeIndex[newType] = newArchetype;
        componentIndex[componentID].emplace(newArchetype);
      }
      oldArchetype.edges.emplace(componentID, newArchetype);
    }

    newArchetype->addValue<Component>(record.archetype, component, record.row);
    record.archetype = *newArchetype;

    // update the EntityIndex map
    entityIndex[entity].archetype = *newArchetype;
    entityIndex[entity].row = newArchetype->components[0].size();
    // place it inside
  }

  template <typename Component>
  void updateComponent(Component component, EntityID entity) {
    Record entityRecord = entityIndex[entity];
    Archetype entityArchetype = entityRecord.archetype;
    Column componentColumn =
        entityArchetype.components[entityArchetype.type.find(
            ComponentIDGenerator::getComponentID<Component>())];
    componentColumn.updateElement<Component>(component, entityRecord.row);
  }

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
    ComponentID componentID = ComponentIDGenerator::getComponentID<C>();
    ArchetypeSet resultSet = componentIndex[componentID];

    ((resultSet = intersect(
          resultSet,
          componentIndex[ComponentIDGenerator::getComponentID<Components>()])),
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
          [](ComponentID component1, ComponentID component2) {
            return component1 ^ component2;
          });
    }
  };

  std::unordered_map<Type, Archetype, TypeHasher> archetypeIndex;

  // Find the archetypes for a component
  std::unordered_map<ComponentID, ArchetypeSet> componentIndex;
  ComponentIDGenerator componentIDGenerator;
};
}  // namespace ecs
