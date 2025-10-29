#include "descriptor_set_pool.hpp"

#include <memory>
#include <vector>

namespace rlm {
// Builder
DescriptorSetPool::Builder &DescriptorSetPool::Builder::addPoolSize(
    VkDescriptorType descriptorType,
    uint32_t count) {
  poolSizes.push_back({descriptorType, count});
  return *this;
}

DescriptorSetPool::Builder &
DescriptorSetPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags) {
  poolFlags = flags;
  return *this;
}

DescriptorSetPool::Builder &
DescriptorSetPool::Builder::setMaxSets(uint32_t count) {
  maxSets = count;
  return *this;
}

std::unique_ptr<DescriptorSetPool> DescriptorSetPool::Builder::build() {
  return std::make_unique<DescriptorSetPool>(
      lveDevice, maxSets, poolFlags, poolSizes);
}

// Descriptor Set Pool

DescriptorSetPool::DescriptorSetPool(
    Device &rlmDevice,
    uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    std::vector<VkDescriptorPoolSize> &poolSizes)
    : rlmDevice(rlmDevice) {
  // The structure has an optional flag similar to command pools that determines
  // if individual descriptor sets can be freed or not:
  // VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT. We're not going to touch
  // the descriptor set after creating it, so we don't need this flag.
  VkDescriptorPoolCreateInfo descriptorPoolInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .flags = poolFlags,
      .maxSets = maxSets,
      .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
      .pPoolSizes = poolSizes.data(),
  };

  auto result = vkCreateDescriptorPool(
      rlmDevice.getDevice(), &descriptorPoolInfo, nullptr, &descriptorPool);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}
}  // namespace rlm
