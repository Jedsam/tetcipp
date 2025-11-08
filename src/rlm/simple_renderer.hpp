#pragma once

#include <memory>

#include "descriptor_set/descriptor_set_layout.hpp"
#include "device.hpp"
#include "model.hpp"
#include "pipeline.hpp"

namespace rlm {

class SimpleRenderSystem {
 public:
  SimpleRenderSystem(
      Device &device,
      VkRenderPass renderPass,
      const DescriptorSetLayout &globalLayout);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;
  void renderGameObjects(VkCommandBuffer commandBuffer, Model &model);

 private:
  void createPipeline(VkRenderPass renderPass);
  void createPipelineLayout(DescriptorSetLayout globalLayout);

  Device &rlmDevice;
  std::unique_ptr<Pipeline> rlmPipeline;
  VkPipelineLayout pipelineLayout;
};

}  // namespace rlm
