#pragma once

#include <cstdint>
#include <memory>

#include "device.hpp"
#include "engine/component/UniformBufferObjectComponent.hpp"
#include "renderer.hpp"
#include "rlm/descriptor_set/descriptor_set.hpp"
#include "rlm/simple_renderer.hpp"
#include "window.hpp"

// Stands for Renderer Little Machine
namespace rlm {

const uint32_t WIDTH = 1200;
const uint32_t HEIGHT = 900;

class Core {
 public:
  Core();
  ~Core();

  bool shouldClose() { return rlmWindow->shouldClose(); }

  void beginFrameOperations();
  void endFrameOperations();

  void waitForDevice() { vkDeviceWaitIdle(rlmDevice->getDevice()); }

  Device &getDevice() { return *rlmDevice; }

  Renderer &getRenderer() { return *rlmRenderer; }

  SimpleRenderSystem &getSimpleRenderSystem() { return *simpleRenderSystem; }

  void updateGlobalUbo(
      const engine::component::UniformBufferObject &uniformBufferObject) {
    int frameIndex = rlmRenderer->getFrameIndex();
    uboSet->updateSet(&uniformBufferObject, frameIndex);
  }

 private:
  std::unique_ptr<Window> rlmWindow;
  std::unique_ptr<Device> rlmDevice;
  std::unique_ptr<Renderer> rlmRenderer;
  std::unique_ptr<SimpleRenderSystem> simpleRenderSystem;
  std::unique_ptr<DescriptorSet> uboSet;

  void init();

  void mainLoop();

  void cleanup();
};

}  // namespace rlm
