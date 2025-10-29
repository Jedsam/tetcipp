
#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "rlm/device.hpp"

namespace rlm {

class DescriptorSetPool {
 public:
  class Builder {
   public:
    explicit Builder(Device &rlmDevice);

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<DescriptorSetPool> build();

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
      std::vector<VkDescriptorPoolSize> &poolSizes);
  ~DescriptorSetPool();

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout,
      VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  Device &getRLMDevice() { return rlmDevice; }

  void resetPool();

  VkDescriptorPool getDescriptorPool() { return descriptorPool; }

 private:
  Device &rlmDevice;
  VkDescriptorPool descriptorPool;

  friend class LveDescriptorWriter;
};
}  // namespace rlm
