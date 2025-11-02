#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "descriptor_set_layout.hpp"
#include "rlm/buffer.hpp"
#include "rlm/descriptor_set/descriptor_set_pool.hpp"

namespace rlm {
struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

class DescriptorSet {
 public:
  class Builder {
   public:
    explicit Builder(Device &rlmDevice);
    ~Builder();

    Builder &addDescriptorSetLayout(
        std::unique_ptr<DescriptorSetLayout> descriptorSetLayout);
    Builder &
    addDescriptorSetPool(std::unique_ptr<DescriptorSetPool> descriptorSetPool);
    Builder &createBufferMemory(uint32_t bufferSize, uint32_t bufferCount);
    std::unique_ptr<DescriptorSet> build();

   private:
    Device &rlmDevice;
    std::vector<std::unique_ptr<Buffer>> buffers;
    std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
    std::unique_ptr<DescriptorSetPool> descriptorSetPool;
  };

  std::unique_ptr<DescriptorSetLayout> &getDescriptorSetLayout() {
    return descriptorSetLayout;
  }

  std::unique_ptr<DescriptorSetPool> &getDescriptorSetPool() {
    return descriptorSetPool;
  }

  std::vector<std::unique_ptr<Buffer>> &getDescriptorBuffers() {
    return buffers;
  }

  std::vector<VkDescriptorSet> &getDescriptorSets() { return descriptorSets; }

  void bindDescriptorSets(
      VkCommandBuffer &commandBuffer,
      VkPipelineBindPoint pipelineBindPoint,
      uint32_t setNumber,
      VkPipelineLayout &layout);

  DescriptorSet(
      Device &rlmDevice,
      std::vector<std::unique_ptr<Buffer>> buffers,
      std::unique_ptr<DescriptorSetPool> pool,
      std::unique_ptr<DescriptorSetLayout> layout);

 private:
  Device &rlmDevice;
  std::vector<std::unique_ptr<Buffer>> buffers;
  std::vector<VkDescriptorSet> descriptorSets;
  std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
  std::unique_ptr<DescriptorSetPool> descriptorSetPool;
};
}  // namespace rlm
