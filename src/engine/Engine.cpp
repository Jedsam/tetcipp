#include <sys/types.h>

#include <cassert>
#include <chrono>
#include <iostream>

#include "Engine.hpp"

namespace engine {

Engine::Engine() : rlmCore(), myRegister() {}

void Engine::run() {
  auto currentTime = std::chrono::high_resolution_clock::now();
  auto lastTime = std::chrono::high_resolution_clock::now();

  int frameCount = 0;
  while (!rlmCore.shouldClose()) {
    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(
            newTime - currentTime)
            .count();
    float elapsed = std::chrono::duration<float, std::chrono::seconds::period>(
                        newTime - lastTime)
                        .count();
    currentTime = newTime;

    frameCount++;

    for (auto &system : systems) {
      system.update(myRegister, elapsed);
    }

    rlmCore.beginFrameOperations();

    renderingSystem->update(myRegister, elapsed);

    rlmCore.endFrameOperations();

    if (elapsed >= 1.0f) {
      float fps = frameCount / elapsed;
      std::cout << "FPS: " << fps << std::endl;

      frameCount = 0;
      lastTime = currentTime;
    }
  }

  rlmCore.waitForDevice();
}
}  // namespace engine
