#include <fstream>
#include <string>
#include <vector>

#include "rlm_pipeline.hpp"

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace rlm {
using std::string;

RLMPipeline::RLMPipeline(
    RLMDevice &rlmDevice,
    const std::string &vertFilePath,
    const std::string &fragFilePath)
    : rlmDevice(rlmDevice) {
  createGraphicsPipeline(vertFilePath, fragFilePath);
}

RLMPipeline::~RLMPipeline() {
  vkDestroyShaderModule(rlmDevice.getDevice(), fragShaderModule, nullptr);
  vkDestroyShaderModule(rlmDevice.getDevice(), vertShaderModule, nullptr);
}

std::vector<char> RLMPipeline::readFile(const std::string &filename) {
  std::ifstream file(ENGINE_DIR + filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

void RLMPipeline::createGraphicsPipeline(const string &vertFilePath, const std::string &fragFilePath) {
  auto vertShaderCode = readFile(vertFilePath);
  auto fragShaderCode = readFile(fragFilePath);

  createShaderModule(vertShaderCode, &vertShaderModule);
  createShaderModule(fragShaderCode, &fragShaderModule);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";
  // vertShaderStageInfo.pSpecializationInfo;

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";
  // fragShaderStageInfo.pSpecializationInfo;
  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
}

void RLMPipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  auto result = vkCreateShaderModule(rlmDevice.getDevice(), &createInfo, nullptr, shaderModule);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }
}
}  // namespace rlm
