#pragma once

#include <glm/glm.hpp>

namespace rlm {
struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

class RLMDescriptor {
 public:
  class Builder {};

 private:
};
}  // namespace rlm
