#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "renderer/rlm_device.hpp"

namespace rlm {

class RLMDescriptorSetLayout {
 public:
  class Builder {
   public:
    explicit Builder(RLMDevice &rlmDevice);

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<RLMDescriptorSetLayout> build() const;

   private:
    RLMDevice &rlmDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> setLayoutBindings;
  };

 private:
};
}  // namespace rlm
