#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "descriptor_set.hpp"
#include "rlm/descriptor_set/descriptor_set_layout.hpp"
#include "rlm/descriptor_set/descriptor_set_pool.hpp"
#include "rlm/descriptor_set/descriptor_set_writer.hpp"

namespace rlm {

// Builder
DescriptorSet::Builder::Builder(Device &rlmDevice) : rlmDevice{rlmDevice} {}

DescriptorSet::Builder::~Builder() {}

DescriptorSet::Builder &DescriptorSet::Builder::addDescriptorSetLayout(
    std::unique_ptr<DescriptorSetLayout> descriptorSetLayout) {
  this->descriptorSetLayout = std::move(descriptorSetLayout);
  return *this;
}

DescriptorSet::Builder &DescriptorSet::Builder::addDescriptorSetPool(
    std::unique_ptr<DescriptorSetPool> descriptorSetPool) {
  this->descriptorSetPool = std::move(descriptorSetPool);
  return *this;
}

DescriptorSet::Builder &DescriptorSet::Builder::allocateDescriptorSets() {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorSetPool->getDescriptorPool();
  allocInfo.descriptorSetCount =
      descriptorSetLayout->getDescriptorSetLayout().size();
  allocInfo.pSetLayouts = descriptorSetLayout->getDescriptorSetLayout().data();

  descriptorSets.resize(descriptorSetLayout->getDescriptorSetLayout().size());
  auto result = vkAllocateDescriptorSets(
      rlmDevice.getDevice(), &allocInfo, descriptorSets.data());
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }
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
  return std::make_unique<DescriptorSet>(
      rlmDevice,
      std::move(buffers),
      std::move(descriptorSetPool),
      std::move(descriptorSetLayout));
}

// Descriptor Set
DescriptorSet::DescriptorSet(
    Device &rlmDevice,
    std::vector<std::unique_ptr<Buffer>> buffers,
    std::unique_ptr<DescriptorSetPool> pool,
    std::unique_ptr<DescriptorSetLayout> layout)
    : rlmDevice(rlmDevice), buffers(std::move(buffers)),
      descriptorSetPool(std::move(pool)),
      descriptorSetLayout(std::move(layout)) {
  // VkDescriptorSetAllocateInfo allocInfo{};
  // allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  // allocInfo.descriptorPool = descriptorSetPool->getDescriptorPool();
  // allocInfo.descriptorSetCount =
  //     descriptorSetLayout->getDescriptorSetLayout().size();
  // allocInfo.pSetLayouts =
  // descriptorSetLayout->getDescriptorSetLayout().data();
  //
  // descriptorSets.resize(descriptorSetLayout->getDescriptorSetLayout().size());
  // auto result = vkAllocateDescriptorSets(
  //     rlmDevice.getDevice(), &allocInfo, descriptorSets.data());
  // if (result != VK_SUCCESS) {
  //   throw std::runtime_error("failed to allocate descriptor sets!");
  // }
}

void DescriptorSet::bindDescriptorSets(
    VkCommandBuffer &commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    uint32_t setNumber,
    VkPipelineLayout &layout) {
  vkCmdBindDescriptorSets(
      commandBuffer,
      pipelineBindPoint,
      layout,
      0,
      1,
      &descriptorSets[setNumber],
      0,
      nullptr);
}
}  // namespace rlm
