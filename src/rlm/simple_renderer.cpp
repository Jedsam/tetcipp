
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <memory>
#include <vector>

// libs
// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/glm.hpp>
// #include <glm/gtc/constants.hpp>
// #include <glm/common.hpp>

#include "model.hpp"
#include "pipeline.hpp"
#include "simple_renderer.hpp"

namespace rlm {

SimpleRenderSystem::~SimpleRenderSystem() {
  vkDestroyPipelineLayout(rlmDevice.getDevice(), pipelineLayout, nullptr);
}

SimpleRenderSystem::SimpleRenderSystem(
    Device &device,
    VkRenderPass renderPass,
    DescriptorSetLayout globalLayout)
    : rlmDevice{device} {
  createPipelineLayout(globalLayout);
  createPipeline(renderPass);
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(
      pipelineLayout != nullptr &&
      "Cannot create pipeline before pipeline layout");
  PipelineConfigInfo pipelineConfigInfo{};
  Pipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
  pipelineConfigInfo.renderPass = renderPass;
  pipelineConfigInfo.pipelineLayout = pipelineLayout;
  pipelineConfigInfo.bindingDescription =
      Model::Vertex::getBindingDescriptions();
  pipelineConfigInfo.attributeDescription =
      Model::Vertex::getAttributeDescriptions();

  rlmPipeline = std::make_unique<Pipeline>(
      rlmDevice,
      "shaders/shader.vert.spv",
      "shaders/shader.frag.spv",
      pipelineConfigInfo);
  if (rlmPipeline == nullptr) {
    throw std::runtime_error("Pipeline creation was unsuccessful");
  }
}

void SimpleRenderSystem::renderGameObjects(
    VkCommandBuffer commandBuffer,
    Model &model) {
  rlmPipeline->bindCommandBuffer(commandBuffer);

  model.bind(commandBuffer);
  model.draw(commandBuffer);
}

void SimpleRenderSystem::createPipelineLayout(
    DescriptorSetLayout globalLayout) {
  std::vector<VkDescriptorSetLayout> layoutArray{
      globalLayout.getDescriptorSetLayout()};
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount =
      static_cast<uint32_t>(layoutArray.size());        // Optional
  pipelineLayoutInfo.pSetLayouts = layoutArray.data();  // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0;        // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr;     // Optional

  auto result = vkCreatePipelineLayout(
      rlmDevice.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

}  // namespace rlm
