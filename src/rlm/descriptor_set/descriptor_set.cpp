#include <cstdint>
#include <memory>
#include <vector>

#include "descriptor_set.hpp"

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

std::unique_ptr<DescriptorSet> DescriptorSet::Builder::build() {
  return std::make_unique<DescriptorSet>();
}

// Descriptor Set
}  // namespace rlm
