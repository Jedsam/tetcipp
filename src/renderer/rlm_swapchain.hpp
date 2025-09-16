#pragma once

#include "rlm_device.hpp"
#include <vulkan/vulkan_core.h>

#include <vector>

namespace rlm {
class RLMSwapChain {
 public:
  RLMSwapChain(RLMDevice &rlmDevice, VkExtent2D windowExtent);
  ~RLMSwapChain();

 private:
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  void init();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();

  RLMDevice &rlmDevice;

  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;

  VkRenderPass renderPass;
  VkSwapchainKHR swapChain;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
};
}  // namespace rlm
