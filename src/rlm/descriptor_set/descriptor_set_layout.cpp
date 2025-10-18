#include <algorithm>
#include <iterator>
#include <memory>
#include <unordered_map>
#include <vector>

#include "descriptor_set_layout.hpp"

namespace rlm {
// Builder
DescriptorSetLayout::Builder::Builder(Device &rlmDevice)
    : rlmDevice{rlmDevice} {}

DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::addBinding(
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
  // uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
  // This field is only relevant for image sampling related descriptors
  setLayoutBindings[binding] = layoutBinding;
  return *this;
}

std::unique_ptr<DescriptorSetLayout>
DescriptorSetLayout::Builder::build() const {
  return std::make_unique<DescriptorSetLayout>(rlmDevice, setLayoutBindings);
}

// DescriptorSetLayout
DescriptorSetLayout::DescriptorSetLayout(
    Device &rlmDevice,
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>
        setLayoutBindings)
    : rlmDevice{rlmDevice}, setLayoutBindings{setLayoutBindings} {
  std::vector<VkDescriptorSetLayoutBinding> bindingsVector;

  std::transform(
      setLayoutBindings.begin(),
      setLayoutBindings.end(),
      std::back_inserter(bindingsVector),
      [](auto binding) { return binding.second; });

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindingsVector.size());
  layoutInfo.pBindings = bindingsVector.data();

  if (vkCreateDescriptorSetLayout(
          rlmDevice.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(
      rlmDevice.getDevice(), descriptorSetLayout, nullptr);
}
}  // namespace rlm
