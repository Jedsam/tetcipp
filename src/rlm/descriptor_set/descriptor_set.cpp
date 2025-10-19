#include <cstdint>
#include <memory>
#include <vector>

#include "descriptor_set.hpp"
#include "rlm/descriptor_set/descriptor_set_layout.hpp"
#include "rlm/descriptor_set/descriptor_set_pool.hpp"

namespace rlm {

// Builder
DescriptorSet::Builder::Builder(Device &rlmDevice) : rlmDevice{rlmDevice} {}

DescriptorSet::Builder::~Builder() {}

DescriptorSet::Builder &DescriptorSet::Builder::addDescriptorSetLayout(
    DescriptorSetLayout descriptorSetLayout) {
  return *this;
}

DescriptorSet::Builder &DescriptorSet::Builder::createBufferMemory(
    uint32_t bufferSize,
    uint32_t bufferCount) {
  buffers = std::vector<std::unique_ptr<Buffer>>(bufferCount);
  for (int i = 0; i < buffers.size(); i++) {
    buffers[i] = std::make_unique<Buffer>(
        rlmDevice,
        bufferSize,
        1,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        0);
    // rlmDevice.properties.limits.minUniformBufferOffsetAlignment);
    buffers[i]->mapMemory();
  }
  return *this;
}

DescriptorSet::Builder &DescriptorSet::Builder::allocatePool(
    DescriptorSetPool &descriptorSetPool,
    uint32_t framesInFlight) {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorSetPool.getDescriptorPool();
  allocInfo.descriptorSetCount = framesInFlight;
  allocInfo.pSetLayouts = descriptorSetLayout.data();
}

std::unique_ptr<DescriptorSet> DescriptorSet::Builder::build() {
  return std::make_unique<DescriptorSet>(
      descriptorSetPool, descriptorSetLayout);
}

// Descriptor Set
}  // namespace rlm
