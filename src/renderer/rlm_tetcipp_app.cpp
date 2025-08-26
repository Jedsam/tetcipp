#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cassert>
#include <memory>

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
  window = std::make_unique<RLMWindow>(HEIGHT, WIDTH, "RLMWindow");
  assert(window != nullptr && "RLMWindow creation was unsuccessful");
  device = std::make_unique<RLMDevice>(*window);
  assert(device != nullptr && "RLMDevice creation was unsuccessful");
}

void RLMApplication::mainLoop() {
  while (!window->shouldClose()) {
    glfwPollEvents();
  }
}

void RLMApplication::cleanup() {}

}  // namespace rlm
