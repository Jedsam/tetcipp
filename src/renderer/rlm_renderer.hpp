#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>
#include <vector>

#include "rlm_swapchain.hpp"
#include "rlm_window.hpp"

namespace rlm {
class RLMRenderer {
 public:
  static const int MAX_FRAMES_IN_FLIGHT = 2;

  RLMRenderer(RLMWindow &rlmWindow, RLMDevice &rlmDevice);
  ~RLMRenderer();

  void beginFrame();
  void endFrame();

  void beginRenderPass();
  void endRenderPass();

  VkRenderPass getRenderPass() { return rlmSwapChain->getRenderPass(); }

  VkCommandBuffer getCommandBuffer() { return commandBuffers[currentFrame]; }

 private:
  RLMDevice &rlmDevice;
  RLMWindow &rlmWindow;
  std::shared_ptr<RLMSwapChain> rlmSwapChain;

  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex = 0;
  uint32_t currentFrame = 0;

  void recreateSwapChain();
  void createCommandBuffers();
  void createSyncObjects();

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkSemaphore> submitSempahores;
  std::vector<VkFence> inFlightFences;
};
}  // namespace rlm
