
#include <spdlog/spdlog.h>

#include <memory>

#include "rlm_device.hpp"
#include "rlm_renderer.hpp"
#include "rlm_swapchain.hpp"
#include "rlm_window.hpp"

namespace rlm {

RLMRenderer::RLMRenderer(RLMWindow &rlmWindow, RLMDevice &rlmDevice)
    : rlmWindow(rlmWindow), rlmDevice(rlmDevice) {
  recreateSwapChain();
  createCommandBuffer();
  spdlog::debug("RLMRenderer: Command buffer allocated\n");
}

RLMRenderer::~RLMRenderer() {}

void RLMRenderer::recreateSwapChain() {
  auto extent = rlmWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = rlmWindow.getExtent();
    glfwWaitEvents();
  }
  rlmSwapChain = std::make_unique<RLMSwapChain>(rlmDevice, extent);
}

void RLMRenderer::beginRenderPass() {
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = rlmSwapChain->getRenderPass();
  // currentImageIndex is undefined for now
  renderPassInfo.framebuffer = rlmSwapChain->getFrameBuffer(currentImageIndex);

  auto swapChainExtent = rlmSwapChain->getExtent();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChainExtent;

  VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  // The first parameter for every command is always the command buffer to record the command to. The second
  // parameter specifies the details of the render pass we've just provided. The final parameter controls how
  // the drawing commands within the render pass will be provided. It can have one of two values:
  // -VK_SUBPASS_CONTENTS_INLINE: The render pass commands will be embedded in the primary command buffer
  // itself and no secondary command buffers will be executed.
  // -VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: The render pass commands will be executed from secondary
  // command buffers.
  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapChainExtent.width);
  viewport.height = static_cast<float>(swapChainExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapChainExtent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void RLMRenderer::endRenderPass() { vkCmdEndRenderPass(commandBuffer); }

void RLMRenderer::beginFrame() {
  // The flags parameter specifies how we're going to use the command buffer. The following values are
  // available:
  //
  // -VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing
  // it once.
  // -VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will
  // be entirely within a single render pass.
  // -VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command
  // buffer can be resubmitted while it is also already pending execution.

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;  // Optional
  // The pInheritanceInfo parameter is only relevant for secondary command buffers. It specifies which state
  // to inherit from the calling primary command buffers.
  beginInfo.pInheritanceInfo = nullptr;  // Optional

  auto result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }
}

void RLMRenderer::endFrame() {
  auto result = vkEndCommandBuffer(commandBuffer);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

void RLMRenderer::createCommandBuffer() {
  // The level parameter specifies if the allocated command buffers are primary or secondary command buffers.

  // -VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution, but cannot be called from
  // other command buffers.
  // -VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can be
  // called from primary command buffers.
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = rlmDevice.getCommandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  auto result = vkAllocateCommandBuffers(rlmDevice.getDevice(), &allocInfo, &commandBuffer);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}
}  // namespace rlm
