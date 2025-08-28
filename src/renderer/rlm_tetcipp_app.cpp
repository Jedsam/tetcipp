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
  while (!rlmWindow->shouldClose()) {
    glfwPollEvents();
  }
}

void RLMApplication::cleanup() {}

}  // namespace rlm
