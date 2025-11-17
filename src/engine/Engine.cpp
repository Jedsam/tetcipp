
#include <spdlog/spdlog.h>
#include <sys/types.h>

#include <cassert>
#include <chrono>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine.hpp"
#include "engine/component/UniformBufferObjectComponent.hpp"

namespace engine {

Engine::Engine() : rlmCore(), myRegister() {}

void Engine::run() {
  auto currentTime = std::chrono::high_resolution_clock::now();
  auto lastTime = std::chrono::high_resolution_clock::now();

  component::UniformBufferObject myGlobalUbo{};

  spdlog::debug("Engine: Starting the engine");

  int frameCount = 0;
  while (!rlmCore.shouldClose()) {
    spdlog::debug("Engine: Loop start");
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

    spdlog::debug("Engine: Calculating actions");
    myGlobalUbo.model = glm::rotate(
        glm::mat4(1.0f),
        frameTime * glm::radians(90.0f),
        glm::vec3(0.0f, 0.0f, 1.0f));
    myGlobalUbo.view = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f));
    myGlobalUbo.proj =
        glm::perspective(glm::radians(45.0f), 1200.0f / 800.0f, 0.1f, 10.0f);
    myGlobalUbo.proj[1][1] *= -1;
    for (auto &system : systems) {
      system->update(myRegister, elapsed);
    }

    spdlog::debug("Engine: beginning frame operations");
    rlmCore.beginFrameOperations();

    spdlog::debug("Engine: Updating rendering");
    renderingSystem->update(myRegister, elapsed);

    spdlog::debug("Engine: Ending frame operations");
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
