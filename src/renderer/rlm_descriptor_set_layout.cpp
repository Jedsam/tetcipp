#include "rlm_descriptor_set_layout.hpp"

namespace rlm {
// Builder
RLMDescriptorSetLayout::Builder::Builder(RLMDevice &rlmDevice) : rlmDevice{rlmDevice} {}

RLMDescriptorSetLayout::Builder &RLMDescriptorSetLayout::Builder::addBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count) {
  assert(setLayoutBindings.count(binding) == 0 && "Binding already in use");
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding = 0;
  layoutBinding.descriptorType = descriptorType;
  layoutBinding.descriptorCount = count;
  layoutBinding.stageFlags = stageFlags;
  setLayoutBindings[binding] = layoutBinding;
  return *this;
}
}  // namespace rlm
