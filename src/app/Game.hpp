#pragma once

#include "engine/Engine.hpp"

namespace app {
class Game {
 public:
  Game();
  void run();

 private:
  void setupRenderer();
  void setupSystems();
  void setupScene();

  engine::Engine myEngine;
};
}  // namespace app
