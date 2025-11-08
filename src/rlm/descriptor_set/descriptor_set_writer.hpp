#pragma once

#include "rlm/descriptor_set/descriptor_set.hpp"
#include "rlm/descriptor_set/descriptor_set_layout.hpp"
#include "rlm/descriptor_set/descriptor_set_pool.hpp"
#include <vulkan/vulkan_core.h>

#include <vector>

#include <glm/glm.hpp>

namespace rlm {

class DescriptorSetWriter {
 public:
  explicit DescriptorSetWriter(DescriptorSet &descriptorSet);
  DescriptorSetWriter &operator=(const DescriptorSetWriter &) = delete;
  DescriptorSetWriter &operator=(DescriptorSetWriter &&) = delete;

  DescriptorSetWriter &writeBuffer(uint32_t binding);
  // DescriptorSetWriter &
  // writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build();
  void overwrite(VkDescriptorSet &set);

 private:
  DescriptorSet &descriptorSet;
  DescriptorSetPool &descriptorSetPool;
  DescriptorSetLayout &descriptorSetLayout;
  std::vector<std::vector<VkWriteDescriptorSet>> writes;
};
}  // namespace rlm
