#include "renderer/rlm_model.hpp"
#include <vulkan/vulkan_core.h>

namespace rlm {
void RLMModel::draw(VkCommandBuffer commandBuffer) {
  // vkCmdDraw has the following parameters, aside from the command buffer:
  //
  // vertexCount: Even though we don't have a vertex buffer, we technically still have 3 vertices to draw.
  // instanceCount: Used for instanced rendering, use 1 if you're not doing that.
  // firstVertex: Used as an offset into the vertex buffer, defines the lowest value of gl_VertexIndex.
  // firstInstance: Used as an offset for instanced rendering, defines the lowest value of gl_InstanceIndex.
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
}  // namespace rlm
