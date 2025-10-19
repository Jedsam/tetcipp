
#pragma once

#include <cstdint>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <unordered_map>

#include <vector>

#include <glm/glm.hpp>

#include "rlm/descriptor_set/descriptor_set_layout.hpp"
#include "rlm/device.hpp"

namespace rlm {

class DescriptorSetPool {
 public:
  class Builder {
   public:
    explicit Builder(Device &rlmDevice);

    Builder &setMaxSets(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<DescriptorSetPool> build() const;

   private:
    Device &lveDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  DescriptorSetPool(
      Device &rlmDevice,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~DescriptorSetPool();
  DescriptorSetPool(const DescriptorSetPool &) = delete;
  DescriptorSetPool &operator=(const DescriptorSetPool &) = delete;

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout,
      VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

  VkDescriptorPool getDescriptorPool() { return descriptorPool; }

 private:
  Device &rlmDevice;
  VkDescriptorPool descriptorPool;

  friend class LveDescriptorWriter;
};
}  // namespace rlm
