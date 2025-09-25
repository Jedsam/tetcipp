#include "renderer/ecs_register.hpp"

namespace ecs {
Entity Register::createEntity() {
  Entity entity;
  if (emptyIds.empty()) {
    entity.setId(nextId++);
    entities.push_back(entity);
  } else {
    entity = entities[emptyIds.at(emptyIds.size() - 1)];
    entity.setGen(entity.getGen() + 1);
  }
  return entity;
}

}  // namespace ecs
