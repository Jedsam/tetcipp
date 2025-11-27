#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>
#include <vector>

#include "rlm/descriptor_set/descriptor_set.hpp"
#include "swapchain.hpp"
#include "window.hpp"

namespace rlm {
class Renderer {
 public:
  static const int MAX_FRAMES_IN_FLIGHT = 2;

  Renderer(Window &rlmWindow, Device &rlmDevice, DescriptorSet &uboSet);
  ~Renderer();

  void beginFrame();
  void endFrame();

  void beginRenderPass();
  void endRenderPass();

  VkRenderPass getRenderPass() { return rlmSwapChain->getRenderPass(); }

  DescriptorSet &getUboSet() { return uboSet; }

  int getFrameIndex() const { return currentFrame; }

  VkCommandBuffer getCommandBuffer() { return commandBuffers[currentFrame]; }

 private:
  Device &rlmDevice;
  Window &rlmWindow;
  DescriptorSet &uboSet;
  std::shared_ptr<SwapChain> rlmSwapChain;

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
