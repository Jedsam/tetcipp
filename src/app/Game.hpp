#pragma once

#include "engine/Engine.hpp"

namespace app {
class Game {
 public:
  Game();
  void run();

 private:
  void setupScene();
  void setupRenderer();

  engine::Engine myEngine;
};
}  // namespace app
