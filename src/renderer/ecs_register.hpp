#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "ecs_entity.hpp"

namespace ecs {
template <typename T> struct ComponentSparseSet {
  std::vector<T> dense_components;
  std::vector<int> sparse_indices;
};

class Register {
 public:
  Entity createEntity();

 private:
  Entity::id_t nextId = 1;
  std::vector<Entity> entities;
  std::vector<Entity::id_t> emptyIds;
};
}  // namespace ecs
