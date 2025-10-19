#pragma once

#include "ecs/register.hpp"
#include "engine/component/ModelComponent.hpp"
#include "engine/component/UniformBufferObjectComponent.hpp"
#include "engine/system/System.hpp"

namespace engine::system {
class RotationSystem : public engine::system::System {
 public:
  RotationSystem() {}

  void update(ecs::Register &register_, float deltaTime) override {
    // Iterate over entities with ModelComponent
    auto components = register_.findArchetypes<component::ModelComponent>();
    for (auto &compArch : components) {
      for (auto &comp :
           compArch->findComponents<component::UniformBufferObject>()) {
      }
    }
  }
};

}  // namespace engine::system
