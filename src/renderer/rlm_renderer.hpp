#pragma once

#include <memory>

#include "rlm_swapchain.hpp"
#include "rlm_window.hpp"

namespace rlm {
class RLMRenderer {
 public:
  RLMRenderer(RLMWindow &rlmWindow, RLMDevice &rlmDevice);
  ~RLMRenderer();

  void beginFrame();
  void endFrame();

  void beginRenderPass();
  void endRenderPass();

 private:
  RLMDevice &rlmDevice;
  RLMWindow &rlmWindow;
  std::unique_ptr<RLMSwapChain> rlmSwapChain;

  VkCommandBuffer commandBuffer;

  uint32_t currentImageIndex;
  void recreateSwapChain();
  void createCommandBuffer();
};
}  // namespace rlm
