
#include <memory>
#include <vector>

#include "engine/components/MeshRenderer.hpp"
#include "rlm/model.hpp"

#include "Game.hpp"

namespace app {
Game::Game() : myEngine(), myRegister() {}

void Game::run() { setupScene(); }

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
  component::ModelComponent myTriangle{};
  myTriangle.model =
      std::make_shared<rlm::Model>(myEngine.getDevice(), modelBuilder);

  myRegister.createEntity<component::ModelComponent>(myTriangle);

  RenderSystem renderSystem();
}
}  // namespace app
