#include <vulkan/vulkan_core.h>

#include <cstring>

#include "renderer/rlm_device.hpp"
#include "rlm_buffer.hpp"

namespace rlm {
RLMBuffer::RLMBuffer(
    RLMDevice &rlmDevice,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment)
    : rlmDevice{rlmDevice}, instanceSize{instanceSize}, instanceCount{instanceCount}, usageFlags{usageFlags},
      memoryPropertyFlags{memoryPropertyFlags} {
  // alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
  // bufferSize = alignmentSize * instanceCount;
  rlmDevice.createBuffer(instanceSize, usageFlags, memoryPropertyFlags, buffer, vertexBufferMemory);
}

RLMBuffer::~RLMBuffer() {
  unmapMemory();
  vkDestroyBuffer(rlmDevice.getDevice(), buffer, nullptr);
  vkFreeMemory(rlmDevice.getDevice(), vertexBufferMemory, nullptr);
}

void RLMBuffer::mapMemory(VkDeviceSize size, VkDeviceSize offset) {
  // It is also possible to specify the special value VK_WHOLE_SIZE to map all of the memory
  vkMapMemory(rlmDevice.getDevice(), vertexBufferMemory, 0, bufferSize, 0, &mappedData);
}

void RLMBuffer::unmapMemory() {
  // It is also possible to specify the special value VK_WHOLE_SIZE to map all of the memory
  vkUnmapMemory(rlmDevice.getDevice(), vertexBufferMemory);
}

void RLMBuffer::writeToBuffer(const void *data, VkDeviceSize size, VkDeviceSize offset) {
  memcpy(mappedData, data, bufferSize);
}
}  // namespace rlm
