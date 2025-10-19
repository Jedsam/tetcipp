#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "descriptor_set_layout.hpp"
#include "rlm/buffer.hpp"
#include "rlm/descriptor_set/descriptor_set_pool.hpp"

namespace rlm {
struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

class DescriptorSet {
 public:
  class Builder {
   public:
    explicit Builder(Device &rlmDevice);
    ~Builder();

    Builder &addDescriptorSetLayout(DescriptorSetLayout descriptorSetLayout);
    Builder &createBufferMemory(uint32_t bufferSize, uint32_t bufferCount);
    Builder &allocatePool(DescriptorSetPool descriptorSetPool);
    std::unique_ptr<DescriptorSet> build();

   private:
    Device &rlmDevice;
    std::vector<std::unique_ptr<Buffer>> buffers;
  };

 private:
};
}  // namespace rlm
