#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

#include <glm/glm.hpp>

namespace rlm {

class RLMModel {
 public:
  struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
  };

  void draw(VkCommandBuffer commandBuffer);

 private:
};
}  // namespace rlm
