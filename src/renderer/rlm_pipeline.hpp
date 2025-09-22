#pragma once

#include <vulkan/vulkan_core.h>

#include <string>
#include <vector>

#include "renderer/rlm_device.hpp"

namespace rlm {

struct PipelineConfigInfo {
  PipelineConfigInfo() = default;
  PipelineConfigInfo(const PipelineConfigInfo &) = delete;
  PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

  std::vector<VkVertexInputBindingDescription> bindingDescription{};
  std::vector<VkVertexInputAttributeDescription> attributeDescription{};
  VkPipelineViewportStateCreateInfo viewportInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  std::vector<VkDynamicState> dynamicStateEnables;
  VkPipelineDynamicStateCreateInfo dynamicStateInfo;
  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class RLMPipeline {
 public:
  RLMPipeline(
      RLMDevice &rlmDevice,
      const std::string &vertFilePath,
      const std::string &fragFilePath,
      const PipelineConfigInfo &configInfo);
  ~RLMPipeline();

  // Delete assignment and clone
  RLMPipeline(const RLMPipeline &) = delete;
  RLMPipeline &operator=(const RLMPipeline &) = delete;

  static void defaultPipelineConfigInfo(PipelineConfigInfo &pipelineConfigInfo);
  static void enableAlphaBlending(PipelineConfigInfo &pipelineConfigInfo);

  void bindCommandBuffer(VkCommandBuffer commandBuffer);

 private:
  static std::vector<char> readFile(const std::string &filepath);

  void createGraphicsPipeline(
      const std::string &vertFilePath,
      const std::string &fragFilePath,
      const PipelineConfigInfo &configInfo);
  void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;

  VkPipeline graphicsPipeline;
  VkPipelineLayout pipelineLayout;

  RLMDevice &rlmDevice;
};

}  // namespace rlm
