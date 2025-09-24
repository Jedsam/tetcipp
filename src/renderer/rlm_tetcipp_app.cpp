#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include "renderer/rlm_model.hpp"
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
  auto lastTime = std::chrono::high_resolution_clock::now();

  const std::vector<RLMModel::Vertex> vertices = {
      {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
  };
  const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
  RLMModel::Builder modelBuilder{};
  modelBuilder.vertices = vertices;
  modelBuilder.indices = indices;
  RLMModel tempModel{*rlmDevice, modelBuilder};

  int frameCount = 0;
  while (!rlmWindow->shouldClose()) {
    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    float elapsed = std::chrono::duration<float, std::chrono::seconds::period>(newTime - lastTime).count();
    currentTime = newTime;

    frameCount++;

    rlmRenderer->beginFrame();
    rlmRenderer->beginRenderPass();

    simpleRenderSystem.renderGameObjects(rlmRenderer->getCommandBuffer(), tempModel);
    // record stuff
    rlmRenderer->endRenderPass();
    rlmRenderer->endFrame();
    glfwPollEvents();

    if (elapsed >= 1.0f) {
      float fps = frameCount / elapsed;
      std::cout << "FPS: " << fps << std::endl;

      frameCount = 0;
      lastTime = currentTime;
    }
  }

  vkDeviceWaitIdle(rlmDevice->getDevice());
}

void RLMApplication::cleanup() {}

}  // namespace rlm
