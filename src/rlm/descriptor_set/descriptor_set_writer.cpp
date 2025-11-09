#include "descriptor_set_writer.hpp"
#include "rlm/descriptor_set/descriptor_set.hpp"
#include "rlm/descriptor_set/descriptor_set_layout.hpp"
#include "rlm/descriptor_set/descriptor_set_pool.hpp"
#include <memory>
#include <vector>

namespace rlm {

DescriptorSetWriter::DescriptorSetWriter(DescriptorSet &descriptorSet)
    : descriptorSet{descriptorSet},
      descriptorSetLayout{*descriptorSet.getDescriptorSetLayout()},
      descriptorSetPool{*descriptorSet.getDescriptorSetPool()} {
  auto &buffers = descriptorSet.getDescriptorBuffers();
  writes.resize(buffers.size());
  bufferInfos.resize(buffers.size());
}

DescriptorSetWriter &DescriptorSetWriter::writeBuffer(uint32_t binding) {
  assert(
      descriptorSetLayout.setLayoutBindings.count(binding) == 1 &&
      "Layout does not contain specified binding");

  auto &bindingDescription = descriptorSetLayout.setLayoutBindings[binding];

  assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");

  auto &buffers = descriptorSet.getDescriptorBuffers();
  auto &sets = descriptorSet.getDescriptorSets();

  for (int i = 0; i < buffers.size(); i++) {
    auto bufferInfo = buffers[i]->descriptorInfo();
    bufferInfos[i].push_back(bufferInfo);

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = sets[i];
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = bindingDescription.descriptorType;
    write.pImageInfo = nullptr;
    write.pBufferInfo = &bufferInfos[i].back();
    write.pTexelBufferView = nullptr;

    writes[i].push_back(write);
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
  auto &sets = descriptorSet.getDescriptorSets();

  assert(sets.size() == writes.size() && "Mismatched sets and writes count");

  for (int i = 0; i < sets.size(); i++) {
    if (!writes[i].empty()) {
      vkUpdateDescriptorSets(
          descriptorSetPool.getRLMDevice().getDevice(),
          static_cast<uint32_t>(writes[i].size()),
          writes[i].data(),
          0,
          nullptr);
    }
  }
  return true;
}

void DescriptorSetWriter::overwrite(VkDescriptorSet &set) {
  for (int i = 0; i < writes.size(); i++) {
    for (auto &write : writes[i]) {
      write.dstSet = set;
    }
    vkUpdateDescriptorSets(
        descriptorSetPool.getRLMDevice().getDevice(),
        writes[i].size(),
        writes[i].data(),
        0,
        nullptr);
  }
}

}  // namespace rlm
