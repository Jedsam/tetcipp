#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

#include "rlm_swapchain.hpp"

namespace rlm {

RLMSwapChain::RLMSwapChain(RLMDevice &rlmDevice, VkExtent2D windowExtent)
    : rlmDevice{rlmDevice}, swapChainExtent{windowExtent} {
  init();
}

RLMSwapChain::~RLMSwapChain() {
  for (auto framebuffer : swapChainFramebuffers) {
    vkDestroyFramebuffer(rlmDevice.getDevice(), framebuffer, nullptr);
  }

  vkDestroyRenderPass(rlmDevice.getDevice(), renderPass, nullptr);

  for (auto imageView : swapChainImageViews) {
    vkDestroyImageView(rlmDevice.getDevice(), imageView, nullptr);
  }
  vkDestroySwapchainKHR(rlmDevice.getDevice(), swapChain, nullptr);
}

void RLMSwapChain::init() {
  createSwapChain();
  spdlog::debug("RLMSwapChain::init: Created swap chain");
  createImageViews();
  spdlog::debug("RLMSwapChain::init: Created image views");
  createRenderPass();
  spdlog::debug("RLMSwapChain::init: Created render pass");
  createFramebuffers();
  spdlog::debug("RLMSwapChain::init: Created frame buffers");
}

void RLMSwapChain::createFramebuffers() {
  swapChainFramebuffers.resize(swapChainImageViews.size());
  for (size_t i = 0; i < swapChainImageViews.size(); i++) {
    VkImageView attachments[] = {swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = swapChainExtent.width;
    framebufferInfo.height = swapChainExtent.height;
    framebufferInfo.layers = 1;

    auto result =
        vkCreateFramebuffer(rlmDevice.getDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void RLMSwapChain::createRenderPass() {
  VkAttachmentDescription colorAttachment{};
  // same as swap chain
  colorAttachment.format = swapChainImageFormat;
  // For multi sampling
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  // The loadOp and storeOp determine what to do with the data in the attachment before rendering and
  // after rendering. We have the following choices for loadOp:
  //
  // -VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
  // -VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
  // -VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  // There are only two possibilities for the storeOp:
  //
  // -VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
  // -VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering
  // operation
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  // For stencil operations
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  // Textures and framebuffers in Vulkan are represented by VkImage objects with a certain pixel format,
  // however the layout of the pixels in memory can change based on what you're trying to do with an
  // image. Some of the most common layouts are:

  // -VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
  // -VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
  // -VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // The attachment parameter specifies which attachment to reference by its index in the attachment
  // descriptions array The layout specifies which layout we would like the attachment to have during a
  // subpass that uses this reference. Vulkan will automatically transition the attachment to this layout
  // when the subpass is started. We intend to use the attachment to function as a color buffer and the
  // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL layout will give us the best performance, as its name
  // implies.
  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Vulkan may also support compute subpasses in the future, so we have to be explicit about this being a
  // graphics subpass.
  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  // The index of the attachment in this array is directly referenced from the fragment shader with the
  // layout(location = 0) out vec4 outColor directive!
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  // The following other types of attachments can be referenced by a subpass:
  // -pInputAttachments: Attachments that are read from a shader
  // -pResolveAttachments: Attachments used for multisampling color attachments
  // -pDepthStencilAttachment: Attachment for depth and stencil data
  // -pPreserveAttachments: Attachments that are not used by this subpass, but for which the data must be
  // preserved

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  auto result = vkCreateRenderPass(rlmDevice.getDevice(), &renderPassInfo, nullptr, &renderPass);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void RLMSwapChain::createImageViews() {
  swapChainImageViews.resize(swapChainImages.size());
  for (size_t i = 0; i < swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapChainImages[i];
    // How the textures are rendered
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapChainImageFormat;

    // Assign color channels
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    auto result = vkCreateImageView(rlmDevice.getDevice(), &createInfo, nullptr, &swapChainImageViews[i]);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }
  }
}

void RLMSwapChain::createSwapChain() {
  SwapChainSupportDetails swapChainSupport = rlmDevice.getSwapChainSupport();

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
  spdlog::debug("RLMSwapChain::createSwapChain: Chosen swap surface format");
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
  spdlog::debug("RLMSwapChain::createSwapChain: Chosen swap present mode");
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
  spdlog::debug("RLMSwapChain::createSwapChain: Chosen swap extent");

  swapChainImageFormat = surfaceFormat.format;

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

  vkGetSwapchainImagesKHR(rlmDevice.getDevice(), swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(rlmDevice.getDevice(), swapChain, &imageCount, swapChainImages.data());
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
    VkExtent2D actualExtent = swapChainExtent;
    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    spdlog::debug("Created swap extent with width: {} height: {}", actualExtent.width, actualExtent.height);

    return actualExtent;
  }
}

}  // namespace rlm
