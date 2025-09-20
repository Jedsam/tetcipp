
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <memory>

// libs
// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/glm.hpp>
// #include <glm/gtc/constants.hpp>
// #include <glm/common.hpp>

#include "renderer/rlm_model.hpp"
#include "renderer/rlm_pipeline.hpp"
#include "rlm_simple_renderer.hpp"

namespace rlm {

SimpleRenderSystem::~SimpleRenderSystem() {
  vkDestroyPipelineLayout(rlmDevice.getDevice(), pipelineLayout, nullptr);
}

SimpleRenderSystem::SimpleRenderSystem(RLMDevice &device, VkRenderPass renderPass) : rlmDevice{device} {
  createPipelineLayout();
  createPipeline(renderPass);
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
  PipelineConfigInfo pipelineConfigInfo{};
  RLMPipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
  pipelineConfigInfo.renderPass = renderPass;
  pipelineConfigInfo.pipelineLayout = pipelineLayout;
  auto bindingDescription = RLMModel::Vertex::getBindingDescriptions();
  auto attributeDescriptions = RLMModel::Vertex::getAttributeDescriptions();

  rlmPipeline = std::make_unique<RLMPipeline>(
      rlmDevice, "shaders/shader.vert.spv", "shaders/shader.frag.spv", pipelineConfigInfo);
  if (rlmPipeline == nullptr) {
    throw std::runtime_error("RLMPipeline creation was unsuccessful");
  }
}

void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, RLMModel &model) {
  rlmPipeline->bindCommandBuffer(commandBuffer);

  model.bind(commandBuffer);
  model.draw(commandBuffer);
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void SimpleRenderSystem::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;             // Optional
  pipelineLayoutInfo.pSetLayouts = nullptr;          // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0;     // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr;  // Optional

  auto result = vkCreatePipelineLayout(rlmDevice.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

}  // namespace rlm
