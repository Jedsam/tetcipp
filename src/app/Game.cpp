
#include <memory>
#include <vector>

#include "ecs/entity.hpp"
#include "engine/component/ModelComponent.hpp"
#include "engine/component/UniformBufferObjectComponent.hpp"
#include "engine/system/RenderSystem.hpp"
#include "engine/system/RotationSystem.hpp"
#include "rlm/model.hpp"

#include "Game.hpp"

namespace app {
Game::Game() : myEngine() {
  setupRenderer();
  setupSystems();
}

void Game::run() {
  setupScene();
  myEngine.run();
}

void Game::setupRenderer() {
  std::shared_ptr<engine::system::RenderSystem> renderSystem =
      std::make_shared<engine::system::RenderSystem>(
          myEngine.getRenderer(), myEngine.getSimpleRenderSystem());
  myEngine.setRenderingSystem(renderSystem);
}

void Game::setupSystems() {
  myEngine.addSystem(std::make_unique<engine::system::RotationSystem>());
}

void Game::setupScene() {
  const std::vector<rlm::Model::Vertex> vertices = {
      {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
  };
  const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
  rlm::Model::Builder modelBuilder{};
  modelBuilder.vertices = vertices;
  modelBuilder.indices = indices;
  engine::component::ModelComponent myTriangleModel{};
  myTriangleModel.model =
      std::make_shared<rlm::Model>(myEngine.getDevice(), modelBuilder);
  ecs::EntityID myTriangle =
      myEngine.createEntity<engine::component::ModelComponent>(myTriangleModel);
  engine::component::UniformBufferObject myTrianglePosition{};
  myEngine.addComponent<engine::component::UniformBufferObject>(
      myTrianglePosition, myTriangle);
}
}  // namespace app
