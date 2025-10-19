#pragma once

#include "ecs/register.hpp"

namespace engine::system {
class System {
 public:
  virtual ~System() = default;

  // Called every frame
  virtual void update(ecs::Register &register_, float deltaTime) = 0;
};
}  // namespace engine::system
