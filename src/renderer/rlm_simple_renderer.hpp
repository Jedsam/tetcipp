#pragma once

#include <memory>

#include "renderer/rlm_device.hpp"
#include "renderer/rlm_pipeline.hpp"

namespace rlm {

class SimpleRenderSystem {
 public:
  explicit SimpleRenderSystem(RLMDevice &rlmDevice);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

 private:
  void createPipeline();
  void createPipelineLayout();

  RLMDevice &rlmDevice;
  std::unique_ptr<RLMPipeline> rlmPipeline;
  VkPipelineLayout pipelineLayout;
};

}  // namespace rlm
