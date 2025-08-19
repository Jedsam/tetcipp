#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cassert>
#include <memory>

#include "rlm_tetcipp_app.hpp"

namespace renderer {
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
  assert(rlmWindow != nullptr && "RLMWindow creation was unsuccessful");
}

void RLMApplication::mainLoop() {
  while (!rlmWindow->shouldClose()) {
    glfwPollEvents();
  }
}

void RLMApplication::cleanup() {}

}  // namespace renderer
