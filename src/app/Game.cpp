#include "Game.hpp"
#include "engine/components/MeshRenderer.hpp"
#include "rlm/rlm_model.hpp"

namespace app {
Game::Game() : myEngine() { setupScene(); }

void Game::run() {}

void Game::setupScene() {
  const std::vector<rlm::RLMModel::Vertex> vertices = {
      {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
  };
  const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
  component::ModelComponent myTriangle{};
  myTriangle.model =

      myRegistry.createEntity();
}
}  // namespace app
