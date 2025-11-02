#include "descriptor_set_writer.hpp"
#include "rlm/descriptor_set/descriptor_set.hpp"
#include "rlm/descriptor_set/descriptor_set_layout.hpp"
#include "rlm/descriptor_set/descriptor_set_pool.hpp"
#include <memory>
#include <vector>

namespace rlm {

DescriptorSetWriter::DescriptorSetWriter(DescriptorSet *descriptorSet)
    : descriptorSet{descriptorSet},
      descriptorSetLayout{descriptorSet->getDescriptorSetLayout().get()},
      descriptorSetPool{descriptorSet->getDescriptorSetPool().get()} {}

DescriptorSetWriter &DescriptorSetWriter::writeBuffer(uint32_t binding) {
  assert(
      descriptorSetLayout->setLayoutBindings.count(binding) == 1 &&
      "Layout does not contain specified binding");

  auto &bindingDescription = descriptorSetLayout->setLayoutBindings[binding];

  assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");

  auto &buffers = descriptorSet->getDescriptorBuffers();
  auto &sets = descriptorSet->getDescriptorSets();

  for (int i = 0; i < buffers.size(); i++) {
    auto bufferInfo = buffers[i]->descriptorInfo();
    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = sets[i];
    write.dstBinding = binding;
    write.dstArrayElement = binding;
    write.descriptorCount = 1;
    write.descriptorType = bindingDescription.descriptorType;
    write.pImageInfo = nullptr;
    write.pBufferInfo = &bufferInfo;
    write.pTexelBufferView = nullptr;

    writes.push_back(write);
  }

  return *this;
}

// DescriptorSetWriter &DescriptorSetWriter::writeImage(
//     uint32_t binding,
//     VkDescriptorImageInfo *imageInfo) {
//   assert(
//       descriptorSetLayout->setLayoutBindings.count(binding) == 1 &&
//       "Layout does not contain specified binding");
//
//   auto &bindingDescription = descriptorSetLayout->setLayoutBindings[binding];
//
//   assert(
//       bindingDescription.descriptorCount == 1 &&
//       "Binding single descriptor info, but binding expects multiple");
//
//   VkWriteDescriptorSet write{
//       .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
//       .dstBinding = binding,
//       .descriptorCount = 1,
//       .descriptorType = bindingDescription.descriptorType,
//       .pImageInfo = imageInfo,
//   };
//   writes.push_back(write);
//   return *this;
// }

bool DescriptorSetWriter::build() {
  // VkDescriptorSet &set
  std::vector<VkDescriptorSet> descriptorSets =
      descriptorSet->getDescriptorSets();
  for (int i = 0; i < descriptorSets.size(); i++) {
    auto currentSet = descriptorSets[i];
    bool success = descriptorSetPool->allocateDescriptor(
        descriptorSetLayout->getDescriptorSetLayout().at(0), currentSet);
    if (!success) {
      return false;
    }
    overwrite(currentSet);
  }
  return true;
}

void DescriptorSetWriter::overwrite(VkDescriptorSet &set) {
  for (auto &write : writes) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(
      descriptorSetPool->getRLMDevice().getDevice(),
      writes.size(),
      writes.data(),
      0,
      nullptr);
}

}  // namespace rlm
