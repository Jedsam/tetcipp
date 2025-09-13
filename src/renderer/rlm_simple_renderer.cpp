
#include <GLFW/glfw3.h>
#include <memory>
#include <vulkan/vulkan_core.h>

#include <cassert>

// libs
// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/glm.hpp>
// #include <glm/gtc/constants.hpp>
// #include <glm/common.hpp>

#include "rlm_simple_renderer.hpp"

namespace rlm {

SimpleRenderSystem::~SimpleRenderSystem() {
  vkDestroyPipelineLayout(rlmDevice.getDevice(), pipelineLayout, nullptr);
}

SimpleRenderSystem::SimpleRenderSystem(RLMDevice &device) : rlmDevice{device} {
  createPipeline();
  createPipelineLayout();
}

void SimpleRenderSystem::createPipeline() {
  rlmPipeline = std::make_unique<RLMPipeline>(rlmDevice, "shaders/vert.spv", "shaders/frag.spv");
}

void SimpleRenderSystem::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;             // Optional
  pipelineLayoutInfo.pSetLayouts = nullptr;          // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0;     // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr;  // Optional

  if (vkCreatePipelineLayout(rlmDevice.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

}  // namespace rlm
