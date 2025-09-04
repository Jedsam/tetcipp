
#include <GLFW/glfw3.h>
#include <memory>
#include <vulkan/vulkan_core.h>

#include <cassert>

// libs
// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/glm.hpp>
// #include <glm/gtc/constants.hpp>
// #include <glm/common.hpp>

#include "rlm_simple_renderer.hpp"

namespace rlm {

SimpleRenderSystem::~SimpleRenderSystem() {}

SimpleRenderSystem::SimpleRenderSystem(RLMDevice &device) : rlmDevice{device} { createPipeline(); }

void SimpleRenderSystem::createPipeline() {
  rlmPipeline = std::make_unique<RLMPipeline>(rlmDevice, "shaders/vert.spv", "shaders/frag.spv");
}

}  // namespace rlm
