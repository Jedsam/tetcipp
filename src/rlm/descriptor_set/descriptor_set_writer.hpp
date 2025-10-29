#pragma once

#include "rlm/descriptor_set/descriptor_set_layout.hpp"
#include "rlm/descriptor_set/descriptor_set_pool.hpp"
#include <vulkan/vulkan_core.h>

#include <glm/glm.hpp>

namespace rlm {

class DescriptorSetWriter {
 public:
  DescriptorSetWriter(DescriptorSetLayout &setLayout, DescriptorSetPool &pool);

  DescriptorSetWriter &
  writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  DescriptorSetWriter &
  writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  DescriptorSetLayout &setLayout;
  DescriptorSetPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};
}  // namespace rlm
