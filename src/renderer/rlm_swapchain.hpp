#pragma once

#include "rlm_device.hpp"
#include <vulkan/vulkan_core.h>

#include <vector>

namespace rlm {
class RLMSwapChain {
 public:
  RLMSwapChain(RLMDevice &rlmDevice, VkExtent2D windowExtent);
  ~RLMSwapChain();

  VkRenderPass getRenderPass() { return renderPass; }

  VkFramebuffer getFrameBuffer(int imageIndex) { return swapChainFramebuffers[imageIndex]; }

  VkExtent2D getExtent() { return swapChainExtent; }

  VkSwapchainKHR getSwapChain() { return swapChain; }

 private:
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  void init();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createFramebuffers();

  RLMDevice &rlmDevice;

  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkRenderPass renderPass;
  VkSwapchainKHR swapChain;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
};
}  // namespace rlm
