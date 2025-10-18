#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <vector>

#include "rlm_buffer.hpp"
#include "rlm_model.hpp"

namespace rlm {

RLMModel::RLMModel(RLMDevice &rlmDevice, const RLMModel::Builder &builder)
    : rlmDevice{rlmDevice} {
  createVertexBuffer(builder.vertices);
  createIndexBuffer(builder.indices);
}

RLMModel::~RLMModel() {}

void RLMModel::createVertexBuffer(const std::vector<Vertex> &vertices) {
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
  uint32_t vertexSize = sizeof(vertices[0]);

  //  buffer usage flags:
  //
  // VK_BUFFER_USAGE_TRANSFER_SRC_BIT: Buffer can be used as source in a memory
  // transfer operation. VK_BUFFER_USAGE_TRANSFER_DST_BIT: Buffer can be used as
  // destination in a memory transfer operation.

  RLMBuffer stagingBuffer{
      rlmDevice,
      vertexSize,
      vertexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

  stagingBuffer.mapMemory();
  stagingBuffer.writeToBuffer(reinterpret_cast<const void *>(vertices.data()));

  vertexBuffer = std::make_unique<RLMBuffer>(
      rlmDevice,
      vertexSize,
      vertexCount,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  rlmDevice.copyBuffer(
      stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void RLMModel::createIndexBuffer(const std::vector<uint32_t> &indices) {
  indexCount = static_cast<uint32_t>(indices.size());
  hasIndexBuffer = indexCount > 0;

  if (!hasIndexBuffer) {
    return;
  }
  VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
  uint32_t indexSize = sizeof(indices[0]);

  //  buffer usage flags:
  //
  // VK_BUFFER_USAGE_TRANSFER_SRC_BIT: Buffer can be used as source in a memory
  // transfer operation. VK_BUFFER_USAGE_TRANSFER_DST_BIT: Buffer can be used as
  // destination in a memory transfer operation.

  RLMBuffer stagingBuffer{
      rlmDevice,
      indexSize,
      indexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

  stagingBuffer.mapMemory();
  stagingBuffer.writeToBuffer(reinterpret_cast<const void *>(indices.data()));

  indexBuffer = std::make_unique<RLMBuffer>(
      rlmDevice,
      indexSize,
      indexCount,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  rlmDevice.copyBuffer(
      stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

void RLMModel::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer->getBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
  if (hasIndexBuffer) {
    vkCmdBindIndexBuffer(
        commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
  }
}

void RLMModel::draw(VkCommandBuffer commandBuffer) {
  if (hasIndexBuffer) {
    // The first two parameters specify the number of indices and the number of
    // instances. We're not using instancing, so just specify 1 instance. The
    // number of indices represents the number of vertices that will be passed
    // to the vertex shader. The next parameter specifies an offset into the
    // index buffer, using a value of 1 would cause the graphics card to start
    // reading at the second index. The second to last parameter specifies an
    // offset to add to the indices in the index buffer. The final parameter
    // specifies an offset for instancing, which we're not using.
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  } else {
    // vkCmdDraw has the following parameters, aside from the command buffer:
    //
    // vertexCount: Even though we don't have a vertex buffer, we technically
    // still have 3 vertices to draw. instanceCount: Used for instanced
    // rendering, use 1 if you're not doing that. firstVertex: Used as an offset
    // into the vertex buffer, defines the lowest value of gl_VertexIndex.
    // firstInstance: Used as an offset for instanced rendering, defines the
    // lowest value of gl_InstanceIndex.
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
  }
}

std::vector<VkVertexInputBindingDescription>
RLMModel::Vertex::getBindingDescriptions() {
  std::vector<VkVertexInputBindingDescription> bindingDescriptions{
      {
          // The binding parameter specifies the index of the binding in the
          // array of bindings. The stride parameter specifies the number of
          // bytes from one entry to the next, and the inputRate parameter can
          // have one of the following values:
          .binding = 0,
          .stride = sizeof(Vertex),
          // VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each
          // vertex VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry
          // after each instance
          .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
      },
  };

  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription>
RLMModel::Vertex::getAttributeDescriptions() {
  // The binding parameter tells Vulkan from which binding the per-vertex data
  // comes. The location parameter references the location directive of the
  // input in the vertex shader.
  //
  // The format parameter describes the type of data for the attribute. A bit
  // confusingly, the formats are specified using the same enumeration as color
  // formats. The following shader types and formats are commonly used together:
  //
  // float: VK_FORMAT_R32_SFLOAT
  // vec2: VK_FORMAT_R32G32_SFLOAT
  // vec3: VK_FORMAT_R32G32B32_SFLOAT
  // vec4: VK_FORMAT_R32G32B32A32_SFLOAT
  // It is allowed to use more channels than the number of components in the
  // shader, but they will be silently discarded. If the number of channels is
  // lower than the number of components, then the BGA components will use
  // default values of (0, 0, 1). The color type (SFLOAT, UINT, SINT) and bit
  // width should also match the type of the shader input. See the following
  // examples:
  //
  // ivec2: VK_FORMAT_R32G32_SINT, a 2-component vector of 32-bit signed
  // integers uvec4: VK_FORMAT_R32G32B32A32_UINT, a 4-component vector of 32-bit
  // unsigned integers double: VK_FORMAT_R64_SFLOAT, a double-precision (64-bit)
  // float

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
