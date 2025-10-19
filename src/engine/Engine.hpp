#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "ecs/entity.hpp"
#include "ecs/register.hpp"
#include "engine/system/System.hpp"
#include "rlm/core.hpp"

namespace engine {
class Engine {
 public:
  Engine();

  void run();

  rlm::Device &getDevice() { return rlmCore.getDevice(); }

  template <typename Component>
  ecs::EntityID createEntity(Component component) {
    return myRegister.createEntity<Component>(component);
  }

  template <typename Component>
  void addComponent(Component component, ecs::EntityID entity) {
    return myRegister.addComponent<Component>(component, entity);
  }

  void
  setRenderingSystem(std::shared_ptr<engine::system::System> renderingSystem) {
    this->renderingSystem = renderingSystem;
  }

  rlm::Renderer &getRenderer() { return rlmCore.getRenderer(); }

  rlm::SimpleRenderSystem &getSimpleRenderSystem() {
    return rlmCore.getSimpleRenderSystem();
  }

  void addSystem(std::unique_ptr<system::System> system) {
    systems.push_back(std::move(system));
  }

 private:
  std::shared_ptr<engine::system::System> renderingSystem;
  std::vector<std::unique_ptr<system::System>> systems;
  rlm::Core rlmCore;
  ecs::Register myRegister;
};
}  // namespace engine
