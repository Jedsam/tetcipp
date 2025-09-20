#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cassert>
#include <chrono>
#include <memory>

#include "renderer/rlm_simple_renderer.hpp"
#include "rlm_tetcipp_app.hpp"

namespace rlm {
RLMApplication::RLMApplication() {}

RLMApplication::~RLMApplication() {}

void RLMApplication::run() {
  init();
  mainLoop();
  cleanup();
}

void RLMApplication::init() {
  // initialize window
  rlmWindow = std::make_unique<RLMWindow>(HEIGHT, WIDTH, "RLMWindow");
  if (rlmWindow == nullptr) {
    throw std::runtime_error("RLMWindow creation was unsuccessful");
  }

  rlmDevice = std::make_unique<RLMDevice>(*rlmWindow);
  if (rlmDevice == nullptr) {
    throw std::runtime_error("RLMDevice creation was unsuccessful");
  }

  rlmRenderer = std::make_unique<RLMRenderer>(*rlmWindow, *rlmDevice);
  if (rlmDevice == nullptr) {
    throw std::runtime_error("RLMRenderer creation was unsuccessful");
  }
}

void RLMApplication::mainLoop() {
  SimpleRenderSystem simpleRenderSystem{*rlmDevice, rlmRenderer->getRenderPass()};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!rlmWindow->shouldClose()) {
    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    rlmRenderer->beginFrame();
    rlmRenderer->beginRenderPass();

    simpleRenderSystem.renderGameObjects(rlmRenderer->getCommandBuffer());
    // record stuff
    rlmRenderer->endRenderPass();
    rlmRenderer->endFrame();
    glfwPollEvents();
  }

  vkDeviceWaitIdle(rlmDevice->getDevice());
}

void RLMApplication::cleanup() {}

}  // namespace rlm
