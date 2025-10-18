
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>

#include "device.hpp"
#include "renderer.hpp"
#include "swapchain.hpp"
#include "window.hpp"

namespace rlm {

Renderer::Renderer(Window &rlmWindow, Device &rlmDevice)
    : rlmWindow(rlmWindow), rlmDevice(rlmDevice) {
  recreateSwapChain();
  createCommandBuffers();
  spdlog::debug("Renderer: Command buffer allocated\n");
  createSyncObjects();
  spdlog::debug("Renderer: Sync Objects created\n");
}

Renderer::~Renderer() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(
        rlmDevice.getDevice(), renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(
        rlmDevice.getDevice(), imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(rlmDevice.getDevice(), inFlightFences[i], nullptr);
  }
  for (size_t i = 0; i < submitSempahores.size(); i++) {
    vkDestroySemaphore(rlmDevice.getDevice(), submitSempahores[i], nullptr);
  }
}

void Renderer::recreateSwapChain() {
  auto extent = rlmWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = rlmWindow.getExtent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(rlmDevice.getDevice());
  rlmSwapChain = std::make_unique<SwapChain>(rlmDevice, extent, rlmSwapChain);
}

void Renderer::createSyncObjects() {
  imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  submitSempahores.resize(rlmSwapChain->getSwapChainImageCount());
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  // In future might need to fill out semaphoreInfo
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  // Same for fenceInfo except create it to be signaled so for the first frame
  // we are not stuck
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    auto result1 = vkCreateSemaphore(
        rlmDevice.getDevice(),
        &semaphoreInfo,
        nullptr,
        &imageAvailableSemaphores[i]);
    auto result2 = vkCreateSemaphore(
        rlmDevice.getDevice(),
        &semaphoreInfo,
        nullptr,
        &renderFinishedSemaphores[i]);

    auto result3 = vkCreateFence(
        rlmDevice.getDevice(), &fenceInfo, nullptr, &inFlightFences[i]);
    if (result1 != VK_SUCCESS || result2 != VK_SUCCESS ||
        result3 != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores!");
    }
  }
  for (size_t i = 0; i < submitSempahores.size(); i++) {
    auto result1 = vkCreateSemaphore(
        rlmDevice.getDevice(), &semaphoreInfo, nullptr, &submitSempahores[i]);
    if (result1 != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores!");
    }
  }
}

void Renderer::beginFrame() {
  // The vkWaitForFences function takes an array of fences and waits on the host
  // for either any or all of the fences to be signaled before returning. The
  // VK_TRUE we pass here indicates that we want to wait for all fences, but in
  // the case of a single one it doesn't matter. This function also has a
  // timeout parameter that we set to the maximum value of a 64 bit unsigned
  // integer, UINT64_MAX, which effectively disables the timeout.
  vkWaitForFences(
      rlmDevice.getDevice(),
      1,
      &inFlightFences[currentFrame],
      VK_TRUE,
      UINT64_MAX);

  // The first two parameters of vkAcquireNextImageKHR are the logical device
  // and the swap chain from which we wish to acquire an image. The third
  // parameter specifies a timeout in nanoseconds for an image to become
  // available. Using the maximum value of a 64 bit unsigned integer means we
  // effectively disable the timeout.
  auto result = vkAcquireNextImageKHR(
      rlmDevice.getDevice(),
      rlmSwapChain->getSwapChain(),
      UINT64_MAX,
      imageAvailableSemaphores[currentFrame],
      VK_NULL_HANDLE,
      &currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  vkResetFences(rlmDevice.getDevice(), 1, &inFlightFences[currentFrame]);

  vkResetCommandBuffer(commandBuffers[currentFrame], 0);
  recordCommandBuffer(commandBuffers[currentFrame], currentImageIndex);
}

void Renderer::endFrame() {
  auto result = vkEndCommandBuffer(commandBuffers[currentFrame]);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

  VkSemaphore signalSemaphores[] = {submitSempahores[currentImageIndex]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  result = vkQueueSubmit(
      rlmDevice.getGraphicsQueue(),
      1,
      &submitInfo,
      inFlightFences[currentFrame]);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  // The first two parameters specify which semaphores to wait on before
  // presentation can happen, just like VkSubmitInfo. Since we want to wait on
  // the command buffer to finish execution,

  VkSwapchainKHR swapChains[] = {rlmSwapChain->getSwapChain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &currentImageIndex;

  presentInfo.pResults = nullptr;  // Optional
  // There is one last optional parameter called pResults. It allows you to
  // specify an array of VkResult values to check for every individual swap
  // chain if presentation was successful. It's not necessary if you're only
  // using a single swap chain, because you can simply use the return value of
  // the present function.
  result = vkQueuePresentKHR(rlmDevice.getPresentQueue(), &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      rlmWindow.wasWindowResized()) {
    rlmWindow.resetWindowResizedFlag();
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::recordCommandBuffer(
    VkCommandBuffer commandBuffer,
    uint32_t imageIndex) {
  // The flags parameter specifies how we're going to use the command buffer.
  // The following values are available:
  //
  // -VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be
  // rerecorded right after executing it once.
  // -VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary
  // command buffer that will be entirely within a single render pass.
  // -VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command
  // buffer can be resubmitted while it is also already pending execution.
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;  // Optional
  // The pInheritanceInfo parameter is only relevant for secondary command
  // buffers. It specifies which state to inherit from the calling primary
  // command buffers.
  beginInfo.pInheritanceInfo = nullptr;  // Optional

  auto result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }
}

void Renderer::beginRenderPass() {
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

  // The first parameter for every command is always the command buffer to
  // record the command to. The second parameter specifies the details of the
  // render pass we've just provided. The final parameter controls how the
  // drawing commands within the render pass will be provided. It can have one
  // of two values: -VK_SUBPASS_CONTENTS_INLINE: The render pass commands will
  // be embedded in the primary command buffer itself and no secondary command
  // buffers will be executed. -VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS:
  // The render pass commands will be executed from secondary command buffers.
  vkCmdBeginRenderPass(
      commandBuffers[currentFrame],
      &renderPassInfo,
      VK_SUBPASS_CONTENTS_INLINE);
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapChainExtent.width);
  viewport.height = static_cast<float>(swapChainExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapChainExtent;
  vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);
}

void Renderer::endRenderPass() {
  vkCmdEndRenderPass(commandBuffers[currentFrame]);
}

void Renderer::createCommandBuffers() {
  commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  // The level parameter specifies if the allocated command buffers are primary
  // or secondary command buffers.

  // -VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for
  // execution, but cannot be called from other command buffers.
  // -VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can
  // be called from primary command buffers.
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = rlmDevice.getCommandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  auto result = vkAllocateCommandBuffers(
      rlmDevice.getDevice(), &allocInfo, commandBuffers.data());
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}
}  // namespace rlm
