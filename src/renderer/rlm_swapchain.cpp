#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

#include "rlm_swapchain.hpp"

namespace rlm {

RLMSwapChain::RLMSwapChain(RLMDevice &rlmDevice, VkExtent2D windowExtent)
    : rlmDevice{rlmDevice}, windowExtent{windowExtent} {
  init();
}

RLMSwapChain::~RLMSwapChain() { vkDestroySwapchainKHR(rlmDevice.getDevice(), swapChain, nullptr); }

void RLMSwapChain::init() {
  createSwapChain();
  spdlog::debug("RLMSwapChain::init: Created swap chain");
}

void RLMSwapChain::createSwapChain() {
  SwapChainSupportDetails swapChainSupport = rlmDevice.getSwapChainSupport();

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
  spdlog::debug("RLMSwapChain::createSwapChain: Chosen swap surface format");
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
  spdlog::debug("RLMSwapChain::createSwapChain: Chosen swap present mode");
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
  spdlog::debug("RLMSwapChain::createSwapChain: Chosen swap extent");

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = rlmDevice.getSurface();
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = rlmDevice.findPhysicalQueueFamilies();
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;      // Optional
    createInfo.pQueueFamilyIndices = nullptr;  // Optional
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;  // False if you want to get access to the pixels behind a window

  createInfo.oldSwapchain = VK_NULL_HANDLE;

  auto result = vkCreateSwapchainKHR(rlmDevice.getDevice(), &createInfo, nullptr, &swapChain);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }
}

VkSurfaceFormatKHR
RLMSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  auto it = std::ranges::find_if(availableFormats, [](const auto &availableFormat) {
    return (
        availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
  });

  if (it != availableFormats.end()) {
    return *it;
  }

  return availableFormats[0];
}

VkPresentModeKHR
RLMSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
  auto it = std::find_if(
      availablePresentModes.begin(), availablePresentModes.end(), [](const auto &availablePresentMode) {
        return availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR;
      });

  if (it != availablePresentModes.end()) {
    return VK_PRESENT_MODE_MAILBOX_KHR;
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D RLMSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    spdlog::debug(
        "Created swap extent with width: {} height: {}",
        capabilities.currentExtent.width,
        capabilities.currentExtent.height);
    return capabilities.currentExtent;
  } else {
    VkExtent2D actualExtent = windowExtent;
    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    spdlog::debug("Created swap extent with width: {} height: {}", actualExtent.width, actualExtent.height);

    return actualExtent;
  }
}

}  // namespace rlm
