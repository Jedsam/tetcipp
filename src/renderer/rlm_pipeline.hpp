#pragma once

#include <vulkan/vulkan_core.h>

#include <string>

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
  void createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath);

  RLMDevice &rlmDevice;
};

}  // namespace rlm
