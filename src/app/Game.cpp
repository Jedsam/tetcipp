#include "Game.hpp"
#include "engine/components/MeshRenderer.hpp"
#include "rlm/rlm_model.hpp"
#include <memory>

namespace app {
Game::Game() : myEngine(), myRegister() { setupScene(); }

void Game::run() {}

void Game::setupScene() {

  const std::vector<rlm::RLMModel::Vertex> vertices = {
      {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
  };
  const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
  rlm::RLMModel::Builder modelBuilder{};
  modelBuilder.vertices = vertices;
  modelBuilder.indices = indices;
  component::ModelComponent myTriangle{};
  myTriangle.model =
      std::make_shared<rlm::RLMModel>(myEngine.getDevice(), modelBuilder);

  myRegister.createEntity<component::ModelComponent>(myTriangle);
}
}  // namespace app
