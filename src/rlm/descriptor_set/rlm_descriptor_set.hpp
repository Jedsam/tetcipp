#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "rlm/rlm_buffer.hpp"
#include "rlm_descriptor_set_layout.hpp"

namespace rlm {
struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

class RLMDescriptorSet {
 public:
  class Builder {
   public:
    explicit Builder(RLMDevice &rlmDevice);
    ~Builder();

    Builder &addDescriptorSetLayout(RLMDescriptorSetLayout descriptorSetLayout);
    Builder &createBufferMemory(uint32_t bufferSize, uint32_t bufferCount);
    std::unique_ptr<RLMDescriptorSet> build();

   private:
    RLMDevice &rlmDevice;
    std::vector<std::unique_ptr<RLMBuffer>> buffers;
  };

 private:
};
}  // namespace rlm
