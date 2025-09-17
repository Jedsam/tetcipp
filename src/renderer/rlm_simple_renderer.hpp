#pragma once

#include <memory>

#include "renderer/rlm_device.hpp"
#include "renderer/rlm_pipeline.hpp"

namespace rlm {

class SimpleRenderSystem {
 public:
  explicit SimpleRenderSystem(RLMDevice &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;
  void renderGameObjects(VkCommandBuffer commandBuffer);

 private:
  void createPipeline(VkRenderPass renderPass);
  void createPipelineLayout();

  RLMDevice &rlmDevice;
  std::unique_ptr<RLMPipeline> rlmPipeline;
  VkPipelineLayout pipelineLayout;
};

}  // namespace rlm
