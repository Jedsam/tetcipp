#include <vulkan/vulkan_core.h>

#include <cstring>

#include "buffer.hpp"
#include "device.hpp"

namespace rlm {
Buffer::Buffer(
    Device &rlmDevice,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment)
    : rlmDevice{rlmDevice}, bufferSize{instanceSize * instanceCount},
      instanceCount{instanceCount}, usageFlags{usageFlags},
      memoryPropertyFlags{memoryPropertyFlags} {
  // alignmentSize = getAlignment(instanceSize, minOffsetAlignment);

  rlmDevice.createBuffer(
      bufferSize, usageFlags, memoryPropertyFlags, buffer, bufferMemory);
}

Buffer::~Buffer() {
  unmapMemory();
  vkDestroyBuffer(rlmDevice.getDevice(), buffer, nullptr);
  vkFreeMemory(rlmDevice.getDevice(), bufferMemory, nullptr);
}

void Buffer::mapMemory(VkDeviceSize size, VkDeviceSize offset) {
  // It is also possible to specify the special value VK_WHOLE_SIZE to map all
  // of the memory
  vkMapMemory(
      rlmDevice.getDevice(), bufferMemory, 0, bufferSize, 0, &mappedData);
}

void Buffer::unmapMemory() {
  // It is also possible to specify the special value VK_WHOLE_SIZE to map all
  // of the memory
  vkUnmapMemory(rlmDevice.getDevice(), bufferMemory);
}

VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = bufferMemory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return vkFlushMappedMemoryRanges(rlmDevice.getDevice(), 1, &mappedRange);
}

void Buffer::writeToBuffer(
    const void *data,
    VkDeviceSize size,
    VkDeviceSize offset) {
  memcpy(mappedData, data, reinterpret_cast<size_t>(bufferSize));
}

VkDescriptorBufferInfo
Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
  return VkDescriptorBufferInfo{
      buffer,
      offset,
      size,
  };
}

}  // namespace rlm
