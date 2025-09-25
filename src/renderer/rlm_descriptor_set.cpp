#include <cstdint>
#include <memory>
#include <vector>

#include "rlm_buffer.hpp"
#include "rlm_descriptor_set.hpp"

namespace rlm {

// Builder
RLMDescriptorSet::Builder::Builder(RLMDevice &rlmDevice) : rlmDevice{rlmDevice} {}

RLMDescriptorSet::Builder::~Builder() {}

RLMDescriptorSet::Builder &
RLMDescriptorSet::Builder::addDescriptorSetLayout(RLMDescriptorSetLayout descriptorSetLayout) {
  return *this;
}

RLMDescriptorSet::Builder &
RLMDescriptorSet::Builder::createBufferMemory(uint32_t bufferSize, uint32_t bufferCount) {
  buffers = std::vector<std::unique_ptr<RLMBuffer>>(bufferCount);
  for (int i = 0; i < buffers.size(); i++) {
    buffers[i] = std::make_unique<RLMBuffer>(
        rlmDevice, bufferSize, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 0);
    // rlmDevice.properties.limits.minUniformBufferOffsetAlignment);
    buffers[i]->mapMemory();
  }
  return *this;
}

std::unique_ptr<RLMDescriptorSet> RLMDescriptorSet::Builder::build() {}

// Descriptor Set
}  // namespace rlm
