#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>
#include <unordered_map>

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
    std::unique_ptr<DescriptorSetLayout> build() const;

   private:
    Device &rlmDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>
        setLayoutBindings;
  };

  DescriptorSetLayout(
      Device &rlmDevice,
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>
          setLayoutBindings);
  ~DescriptorSetLayout();

  VkDescriptorSetLayout getDescriptorSetLayout() { return descriptorSetLayout; }

 private:
  VkDescriptorSetLayout descriptorSetLayout;
  Device &rlmDevice;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> setLayoutBindings;
};
}  // namespace rlm
