

#pragma once

#include "ecs/register.hpp"
#include "engine/component/ModelComponent.hpp"
#include "engine/system/System.hpp"
#include "rlm/renderer.hpp"
#include "rlm/simple_renderer.hpp"

namespace engine::system {
class RenderSystem : public engine::system::System {
 public:
  RenderSystem(
      rlm::Renderer &rlmRenderer,
      rlm::SimpleRenderSystem &simpleRenderSystem)
      : rlmRenderer(rlmRenderer), simpleRenderSystem(simpleRenderSystem) {}

  void update(ecs::Register &register_, float deltaTime) override {
    // Iterate over entities with ModelComponent
    auto components = register_.findArchetypes<component::ModelComponent>();
    for (auto &compArch : components) {
      for (auto &comp : compArch->findComponents<component::ModelComponent>()) {
        // Render logic here
        simpleRenderSystem.renderGameObjects(
            rlmRenderer.getCommandBuffer(), *comp.model);
      }
    }
  }

 private:
  rlm::Renderer &rlmRenderer;
  rlm::SimpleRenderSystem &simpleRenderSystem;
};

}  // namespace engine::system
