#pragma once

#include <vulkan/vulkan_core.h>

#include <string>
#include <vector>

#include "renderer/rlm_device.hpp"

namespace rlm {

class RLMPipeline {
 public:
  RLMPipeline(RLMDevice &rlmDevice, const std::string &vertFilePath, const std::string &fragFilePath);
  ~RLMPipeline();

  // Delete assignment and clone
  RLMPipeline(const RLMPipeline &) = delete;
  RLMPipeline &operator=(const RLMPipeline &) = delete;

 private:
  static std::vector<char> readFile(const std::string &filepath);

  void createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath);
  void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;

  RLMDevice &rlmDevice;
};

}  // namespace rlm
