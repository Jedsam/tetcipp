#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ecs_component.hpp"
#include "ecs_entity.hpp"

namespace ecs {

using ComponentID = uint32_t;
using ArchetypeId = uint32_t;

struct Register {
 public:
  struct Column {
   public:
    explicit Column(size_t element_size) : element_size(element_size) {
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

    void swapWithLastElement(size_t row) {
      if (count == 0) {
        return;
      }
      count--;
      std::memcpy(at(row), at(count), element_size);

      if (count > 16 && count < capacity / 4) {
        data = reallocarray(data, capacity / 2, element_size);
        capacity /= 2;
      }
      count++;
    }

    template <typename Component>
    void pushBack(const Component &component, size_t index) {
      if (count >= capacity) {
        data = reallocarray(data, capacity * 2, element_size);
        capacity *= 2;
      }
      std::memcpy(at(count), &component, element_size);
      count++;
    }

    void pushBack(Column &inputColumn, size_t index) {
      if (count >= capacity / 2) {
        data = reallocarray(data, capacity * 2, element_size);
        capacity *= 2;
      }
      std::memcpy(at(count), inputColumn.at(index), element_size);
      count++;
    }

    template <typename Component>
    void updateElement(const Component &component, size_t index) {
      if (index >= capacity) {
        return;
      }
      std::memcpy(at(index), component, element_size);
    }

    void deleteElement(size_t index) {
      // replace the element with the top value
      std::memcpy(at(index), at(count - 1), element_size);
      if (count > 16 && count < capacity / 4) {
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

    bool operator==(const Type &other) const {
      return componentIDs == other.componentIDs;
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

    void remove(ComponentID id) {
      auto it = std::lower_bound(componentIDs.begin(), componentIDs.end(), id);

      if (it != componentIDs.end() && *it == id) {
        componentIDs.erase(it);
      }
    }

    ComponentID &operator[](size_t index) { return componentIDs[index]; }

    const ComponentID &operator[](size_t index) const {
      return componentIDs[index];
    }

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
    // Store the entities list
    std::vector<EntityID> entities;
    std::unordered_map<ComponentID, ArchetypeEdge> edges;

    void swapWithLastElement(size_t row) {
      for (int i = 0; i < components.size(); i++) {
        components[i].swapWithLastElement(row);
      }
      entities[row] = entities[entities.size() - 1];
    }

    // Adds a value to the archetype given the archetype the components resides
    // in, row value of the entity components and the new component to add
    template <typename Component>
    void
    copyValue(const Archetype &oldArchetype, Component component, size_t row) {
      auto &newComponents = components;
      auto &newType = type;
      auto &oldComponents = oldArchetype.components;
      auto &oldType = oldArchetype.type;
      for (int i = 0, j = 0; i < newComponents.size(); i++) {
        if (newType[i] == oldType[j]) {
          newComponents[i].pushBack(oldComponents[j], row);
          j++;
        } else {
          newComponents[i].pushBack<Component>(component, row);
        }
      }
      entities.push_back(oldArchetype.entities[row]);
    }

    // Adds a value to the archetype given the archetype the components resides
    // in, row value of the entity components and component that was removed
    // from the oldArchetype as a typename
    void copyValue(const Archetype &oldArchetype, size_t row) {
      auto &newComponents = components;
      auto &newType = type;
      auto &oldComponents = oldArchetype.components;
      auto &oldType = oldArchetype.type;
      for (int i = 0, j = 0; i < oldComponents.size(); i++) {
        if (newType[j] == oldType[i]) {
          newComponents[i].pushBack(oldComponents[j], row);
          j++;
        }
      }
    }
  };

  struct Record {
    Archetype *archetype;
    size_t row;
  };

  // TO DO entity creationg and deletion
  template <typename Component> EntityID createEntity(Component component) {}

  void deleteEntity(EntityID entity);
  bool isEntityAlive(EntityID entity);

  // it isnt safe and might cause unexpected bugs if tried to add components
  // that exist
  template <typename Component>
  void addComponent(Component component, EntityID entity) {
    Record &record = entityIndex[entity];
    auto &edgesMap = record.archetype->edges;
    ComponentID &componentID =
        ComponentIDGenerator::getComponentID<Component>();
    auto it = edgesMap.find(componentID);
    Archetype *oldArchetype = record.archetype;
    Archetype *newArchetype;
    if (it != edgesMap.end()) {
      newArchetype = &it->second.edge;
    } else {
      Type newType = oldArchetype->type.clone();
      newType.add(ComponentIDGenerator::getComponentID<Component>());
      // if it doesnt exist create a new archetype and insert it to the map
      auto [itArche, inserted] =
          archetypeIndex.emplace(std::move(newType), Archetype{});
      newArchetype = &itArche->second;
      if (inserted) {
        newArchetype->type = itArche->first;  // The key Type
        newArchetype->components = newArchetype->type.initComponentVector();
        newArchetype->edges.emplace(componentID, oldArchetype);
        componentIndex[componentID].emplace(newArchetype);
      }
      oldArchetype->edges.emplace(componentID, newArchetype);
    }

    newArchetype->copyValue<Component>(oldArchetype, component, record.row);
    oldArchetype->swapWithLastElement(record.row);
    record.archetype = newArchetype;

    // update the EntityIndex map
    entityIndex[entity].archetype = newArchetype;
    entityIndex[entity].row = newArchetype->components[0].size() - 1;
    // place it inside
  }

  template <typename Component>
  void updateComponent(Component component, EntityID entity) {
    Record &entityRecord = entityIndex[entity];
    Archetype &entityArchetype = *entityRecord.archetype;
    Column &componentColumn =
        entityArchetype.components[entityArchetype.type.find(
            ComponentIDGenerator::getComponentID<Component>())];
    componentColumn.updateElement<Component>(component, entityRecord.row);
  }

  // it isnt safe and might cause unexpected bugs if tried to delete components
  // that doesnt exist
  template <typename Component> void deleteComponent(EntityID entity) {
    Record &record = entityIndex[entity];
    auto &edgesMap = record.archetype->edges;
    ComponentID componentID = ComponentIDGenerator::getComponentID<Component>();
    auto it = edgesMap.find(componentID);
    Archetype *newArchetype;
    Archetype *oldArchetype = record.archetype;
    if (it != edgesMap.end()) {
      newArchetype = &it->second.edge;
    } else {
      Type newType = oldArchetype->type.clone();
      newType.remove(ComponentIDGenerator::getComponentID<Component>());
      auto [itArche, inserted] =
          archetypeIndex.emplace(std::move(newType), Archetype{});
      newArchetype = &itArche->second;
      if (inserted) {
        newArchetype->type = itArche->first;  // The key Type
        newArchetype->components = newArchetype->type.initComponentVector();
        newArchetype->edges.emplace(componentID, oldArchetype);
        componentIndex[componentID].emplace(newArchetype);
      }
      oldArchetype->edges.emplace(componentID, newArchetype);
    }

    newArchetype->copyValue<Component>(record.archetype, record.row);
    oldArchetype->swapWithLastElement(record.row);
    record.archetype = newArchetype;

    // update the EntityIndex map
    entityIndex[entity].archetype = newArchetype;
    entityIndex[entity].row = newArchetype->components[0].size() - 1;
  }

  Archetype *findArchetype(EntityID entity) {
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
