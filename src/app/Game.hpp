#pragma once

#include "ecs/ecs_register.hpp"
#include "engine/Engine.hpp"

namespace app {
class Game {
 public:
  Game();
  void run();

 private:
  void setupScene();

  engine::Engine myEngine;
  ecs::Register myRegistry;
};
}  // namespace app
