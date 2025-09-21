#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "renderer/rlm_buffer.hpp"
#include "renderer/rlm_device.hpp"
#include <glm/glm.hpp>

namespace rlm {

class RLMModel {
 public:
  struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
  };

  struct Builder {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};

    void loadModel(const std::string &fileName);
  };

  RLMModel(RLMDevice &rlmDevice, const RLMModel::Builder &builder);

  ~RLMModel();

  void draw(VkCommandBuffer commandBuffer);

  void bind(VkCommandBuffer commandBuffer);

 private:
  void createVertexBuffer(const std::vector<Vertex> &vertices);
  void createIndexBuffer(const std::vector<uint32_t> &indices);

  RLMDevice &rlmDevice;
  std::unique_ptr<RLMBuffer> vertexBuffer;

  uint32_t vertexCount;
};
}  // namespace rlm
