#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "device.hpp"

namespace rlm {
class SwapChain {
 public:
  SwapChain(Device &rlmDevice, VkExtent2D windowExtent);
  SwapChain(
      Device &rlmDevice,
      VkExtent2D windowExtent,
      std::shared_ptr<SwapChain> oldSwapChain);
  ~SwapChain();

  VkRenderPass getRenderPass() { return renderPass; }

  VkFramebuffer getFrameBuffer(int imageIndex) {
    return swapChainFramebuffers[imageIndex];
  }

  VkExtent2D getExtent() { return swapChainExtent; }

  VkSwapchainKHR getSwapChain() { return swapChain; }

  uint32_t getSwapChainImageCount() { return swapChainImages.size(); }

 private:
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  void init();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createFramebuffers();

  Device &rlmDevice;

  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  std::shared_ptr<SwapChain> oldSwapChain;

  VkRenderPass renderPass;
  VkSwapchainKHR swapChain;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
};
}  // namespace rlm
