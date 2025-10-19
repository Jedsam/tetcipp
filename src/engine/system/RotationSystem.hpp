#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
        comp.model = glm::rotate(
            glm::mat4(1.0f),
            deltaTime * glm::radians(90.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));
        comp.view = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));
        comp.proj = glm::perspective(
            glm::radians(45.0f), 1200.0f / 800.0f, 0.1f, 10.0f);
      }
    }
  }
};

}  // namespace engine::system
