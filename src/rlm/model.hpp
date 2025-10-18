#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "buffer.hpp"
#include "device.hpp"
#include <glm/glm.hpp>

namespace rlm {

class Model {
 public:
  struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription>
    getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescriptions();
  };

  struct Builder {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};

    void loadModel(const std::string &fileName);
  };

  Model(Device &rlmDevice, const Model::Builder &builder);

  ~Model();

  void draw(VkCommandBuffer commandBuffer);

  void bind(VkCommandBuffer commandBuffer);

 private:
  void createVertexBuffer(const std::vector<Vertex> &vertices);
  void createIndexBuffer(const std::vector<uint32_t> &indices);

  Device &rlmDevice;
  std::unique_ptr<Buffer> vertexBuffer;
  std::unique_ptr<Buffer> indexBuffer;

  bool hasIndexBuffer;
  uint32_t vertexCount;
  uint32_t indexCount;
};
}  // namespace rlm
