#include <vulkan/vulkan_core.h>

#include <cstring>

#include "renderer/rlm_device.hpp"
#include "rlm_buffer.hpp"

namespace rlm {
RLMBuffer::RLMBuffer(
    RLMDevice &rlmDevice,
    VkDeviceSize bufferSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment)
    : rlmDevice{rlmDevice}, bufferSize{bufferSize}, instanceCount{instanceCount}, usageFlags{usageFlags},
      memoryPropertyFlags{memoryPropertyFlags} {
  // alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
  // bufferSize = alignmentSize * instanceCount;

  rlmDevice.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, bufferMemory);
}

RLMBuffer::~RLMBuffer() {
  unmapMemory();
  vkDestroyBuffer(rlmDevice.getDevice(), buffer, nullptr);
  vkFreeMemory(rlmDevice.getDevice(), bufferMemory, nullptr);
}

void RLMBuffer::mapMemory(VkDeviceSize size, VkDeviceSize offset) {
  // It is also possible to specify the special value VK_WHOLE_SIZE to map all of the memory
  vkMapMemory(rlmDevice.getDevice(), bufferMemory, 0, bufferSize, 0, &mappedData);
}

void RLMBuffer::unmapMemory() {
  // It is also possible to specify the special value VK_WHOLE_SIZE to map all of the memory
  vkUnmapMemory(rlmDevice.getDevice(), bufferMemory);
}

void RLMBuffer::writeToBuffer(const void *data, VkDeviceSize size, VkDeviceSize offset) {
  memcpy(mappedData, data, reinterpret_cast<size_t>(bufferSize));
}
}  // namespace rlm
