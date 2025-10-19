#pragma once

#include <glm/ext/matrix_float4x4.hpp>

namespace engine::component {
struct UniformBufferObject {
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 proj = glm::mat4(1.0f);
};
}  // namespace engine::component
