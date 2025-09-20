#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

#include "renderer/rlm_buffer.hpp"
#include "renderer/rlm_model.hpp"

namespace rlm {

RLMModel::RLMModel(RLMDevice &rlmDevice, const RLMModel::Builder &builder) : rlmDevice{rlmDevice} {
  createVertexBuffer(builder.vertices);
}

RLMModel::~RLMModel() {}

void RLMModel::createVertexBuffer(const std::vector<Vertex> &vertices) {
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
  uint32_t vertexSize = sizeof(vertices[0]);

  vertexBuffer = std::make_unique<RLMBuffer>(
      rlmDevice,
      bufferSize,
      vertexCount,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);

  vertexBuffer->mapMemory();
  vertexBuffer->writeToBuffer(reinterpret_cast<const void *>(vertices.data()));
}

void RLMModel::createIndexBuffer(const std::vector<uint32_t> &indices) {}

void RLMModel::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer->getBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void RLMModel::draw(VkCommandBuffer commandBuffer) {
  // vkCmdDraw has the following parameters, aside from the command buffer:
  //
  // vertexCount: Even though we don't have a vertex buffer, we technically still have 3 vertices to draw.
  // instanceCount: Used for instanced rendering, use 1 if you're not doing that.
  // firstVertex: Used as an offset into the vertex buffer, defines the lowest value of gl_VertexIndex.
  // firstInstance: Used as an offset for instanced rendering, defines the lowest value of gl_InstanceIndex.
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

VkVertexInputBindingDescription RLMModel::Vertex::getBindingDescriptions() {
  VkVertexInputBindingDescription bindingDescription{};
  // The binding parameter specifies the index of the binding in the array of bindings. The stride parameter
  // specifies the number of bytes from one entry to the next, and the inputRate parameter can have one of the
  // following values:
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  // VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
  // VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> RLMModel::Vertex::getAttributeDescriptions() {
  // The binding parameter tells Vulkan from which binding the per-vertex data comes. The location
  // parameter references the location directive of the input in the vertex shader.
  //
  // The format parameter describes the type of data for the attribute. A bit confusingly, the formats are
  // specified using the same enumeration as color formats. The following shader types and formats are
  // commonly used together:
  //
  // float: VK_FORMAT_R32_SFLOAT
  // vec2: VK_FORMAT_R32G32_SFLOAT
  // vec3: VK_FORMAT_R32G32B32_SFLOAT
  // vec4: VK_FORMAT_R32G32B32A32_SFLOAT
  // It is allowed to use more channels than the number of components in the shader, but they will be
  // silently discarded. If the number of channels is lower than the number of
  // components, then the BGA components will use default values of (0, 0, 1). The color type (SFLOAT, UINT,
  // SINT) and bit width should also match the type of the shader input. See the following examples:
  //
  // ivec2: VK_FORMAT_R32G32_SINT, a 2-component vector of 32-bit signed integers
  // uvec4: VK_FORMAT_R32G32B32A32_UINT, a 4-component vector of 32-bit unsigned integers
  // double: VK_FORMAT_R64_SFLOAT, a double-precision (64-bit) float

  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{
      {
          .location = 0,
          .binding = 0,
          .format = VK_FORMAT_R32G32_SFLOAT,
          .offset = offsetof(Vertex, pos),
      },
      {
          .location = 1,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32_SFLOAT,
          .offset = offsetof(Vertex, color),
      },
  };

  return attributeDescriptions;
}

}  // namespace rlm
