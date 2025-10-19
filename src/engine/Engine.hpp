#pragma once

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
    myRegister.createEntity<Component>(component);
  }

  template <typename Component>
  ecs::EntityID addComponent(Component component, ecs::EntityID entity) {
    myRegister.addComponent<Component>(component, entity);
  }

  void
  setRenderingSystem(std::shared_ptr<engine::system::System> renderingSystem) {
    this->renderingSystem = renderingSystem;
  }

 private:
  std::shared_ptr<engine::system::System> renderingSystem;
  std::vector<system::System> systems;
  rlm::Core rlmCore;
  ecs::Register myRegister;
};
}  // namespace engine
