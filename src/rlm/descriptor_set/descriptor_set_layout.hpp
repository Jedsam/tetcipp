#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "rlm/device.hpp"

namespace rlm {

class DescriptorSetLayout {
 public:
  class Builder {
   public:
    explicit Builder(Device &rlmDevice);

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    Builder &setLayoutCount(size_t layoutCount);
    std::unique_ptr<DescriptorSetLayout> build() const;

   private:
    Device &rlmDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>
        setLayoutBindings;
    size_t layoutCount = 0;
  };

  DescriptorSetLayout(
      Device &rlmDevice,
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>
          setLayoutBindings,
      size_t setLayoutCount);
  ~DescriptorSetLayout();

  std::vector<VkDescriptorSetLayout> &getDescriptorSetLayout() {
    return descriptorSetLayouts;
  }

 private:
  std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
  Device &rlmDevice;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> setLayoutBindings;

  friend class DescriptorSetWriter;
};
}  // namespace rlm
