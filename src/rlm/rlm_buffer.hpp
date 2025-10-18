#pragma once

#include <vulkan/vulkan_core.h>

#include "rlm_device.hpp"

namespace rlm {

class RLMBuffer {
 public:
  RLMBuffer(
      RLMDevice &rlmDevice,
      VkDeviceSize instanceSize,
      uint32_t instanceCount,
      VkBufferUsageFlags usageFlags,
      VkMemoryPropertyFlags memoryPropertyFlags,
      VkDeviceSize minOffsetAlignment = 1);
  ~RLMBuffer();

  void mapMemory(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  void writeToBuffer(
      const void *data,
      VkDeviceSize size = VK_WHOLE_SIZE,
      VkDeviceSize offset = 0);

  VkBuffer getBuffer() { return buffer; }

 private:
  void unmapMemory();

  RLMDevice &rlmDevice;
  void *mappedData = nullptr;
  VkBuffer buffer = VK_NULL_HANDLE;
  VkDeviceMemory bufferMemory = VK_NULL_HANDLE;

  VkDeviceSize bufferSize;
  uint32_t instanceCount;
  VkBufferUsageFlags usageFlags;
  VkMemoryPropertyFlags memoryPropertyFlags;
};
}  // namespace rlm
