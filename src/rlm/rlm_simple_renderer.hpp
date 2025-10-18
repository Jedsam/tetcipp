#pragma once

#include <memory>

#include "descriptor_set/rlm_descriptor_set_layout.hpp"
#include "rlm_device.hpp"
#include "rlm_model.hpp"
#include "rlm_pipeline.hpp"

namespace rlm {

class SimpleRenderSystem {
 public:
  SimpleRenderSystem(
      RLMDevice &device,
      VkRenderPass renderPass,
      RLMDescriptorSetLayout globalLayout);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;
  void renderGameObjects(VkCommandBuffer commandBuffer, RLMModel &model);

 private:
  void createPipeline(VkRenderPass renderPass);
  void createPipelineLayout(RLMDescriptorSetLayout globalLayout);

  RLMDevice &rlmDevice;
  std::unique_ptr<RLMPipeline> rlmPipeline;
  VkPipelineLayout pipelineLayout;
};

}  // namespace rlm
