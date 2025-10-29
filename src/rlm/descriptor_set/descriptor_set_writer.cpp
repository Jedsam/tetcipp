#include "descriptor_set_writer.hpp"

namespace rlm {

DescriptorSetWriter::DescriptorSetWriter(
    DescriptorSetLayout &setLayout,
    DescriptorSetPool &pool)
    : setLayout{setLayout}, pool{pool} {}

DescriptorSetWriter &DescriptorSetWriter::writeBuffer(
    uint32_t binding,
    VkDescriptorBufferInfo *bufferInfo) {
  assert(
      setLayout.setLayoutBindings.count(binding) == 1 &&
      "Layout does not contain specified binding");

  auto &bindingDescription = setLayout.setLayoutBindings[binding];

  assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstBinding = binding,
      .descriptorCount = 1,
      .descriptorType = bindingDescription.descriptorType,
      .pBufferInfo = bufferInfo,
  };

  writes.push_back(write);
  return *this;
}

DescriptorSetWriter &DescriptorSetWriter::writeImage(
    uint32_t binding,
    VkDescriptorImageInfo *imageInfo) {
  assert(
      setLayout.setLayoutBindings.count(binding) == 1 &&
      "Layout does not contain specified binding");

  auto &bindingDescription = setLayout.setLayoutBindings[binding];

  assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstBinding = binding,
      .descriptorCount = 1,
      .descriptorType = bindingDescription.descriptorType,
      .pImageInfo = imageInfo,
  };
  writes.push_back(write);
  return *this;
}

bool DescriptorSetWriter::build(VkDescriptorSet &set) {
  bool success =
      pool.allocateDescriptor(setLayout.getDescriptorSetLayout().at(0), set);
  if (!success) {
    return false;
  }
  overwrite(set);
  return true;
}

void DescriptorSetWriter::overwrite(VkDescriptorSet &set) {
  for (auto &write : writes) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(
      pool.getRLMDevice().getDevice(),
      writes.size(),
      writes.data(),
      0,
      nullptr);
}

}  // namespace rlm
