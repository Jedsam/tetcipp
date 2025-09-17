#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

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

  VkRenderPass getRenderPass() { return rlmSwapChain->getRenderPass(); }

  VkCommandBuffer getCommandBuffer() { return commandBuffer; }

 private:
  RLMDevice &rlmDevice;
  RLMWindow &rlmWindow;
  std::unique_ptr<RLMSwapChain> rlmSwapChain;

  VkCommandBuffer commandBuffer;

  uint32_t currentImageIndex;

  void recreateSwapChain();
  void createCommandBuffer();
  void createSyncObjects();

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  VkSemaphore imageAvailableSemaphore;
  VkSemaphore renderFinishedSemaphore;
  VkFence inFlightFence;
};
}  // namespace rlm
