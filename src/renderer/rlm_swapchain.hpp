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

  RLMDevice &rlmDevice;

  VkSwapchainKHR swapChain;
  VkExtent2D windowExtent;
};
}  // namespace rlm
