#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cassert>
#include <memory>
#include <utility>

#include "core.hpp"
#include "descriptor_set/descriptor_set.hpp"
#include "descriptor_set/descriptor_set_layout.hpp"
#include "descriptor_set/descriptor_set_writer.hpp"
#include "engine/component/UniformBufferObjectComponent.hpp"
#include "renderer.hpp"
#include "rlm/descriptor_set/descriptor_set_writer.hpp"
#include "simple_renderer.hpp"

namespace rlm {
Core::Core() { init(); }

Core::~Core() {}

void Core::init() {
  // initialize window
  rlmWindow = std::make_unique<Window>(HEIGHT, WIDTH, "Window");
  if (rlmWindow == nullptr) {
    throw std::runtime_error("Window creation was unsuccessful");
  }

  rlmDevice = std::make_unique<Device>(*rlmWindow);
  if (rlmDevice == nullptr) {
    throw std::runtime_error("Device creation was unsuccessful");
  }

  rlmRenderer = std::make_unique<Renderer>(*rlmWindow, *rlmDevice);
  if (rlmDevice == nullptr) {
    throw std::runtime_error("Renderer creation was unsuccessful");
  }

  spdlog::debug("Core: RLM objects created successfully");
  auto uboDescriptorSetLayout =
      DescriptorSetLayout::Builder(*rlmDevice)
          .addBinding(
              0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
          .setLayoutCount(Renderer::MAX_FRAMES_IN_FLIGHT)
          .build();
  spdlog::debug("Core: Ubo descriptor set layout done");

  auto uboDescriptorSetPool =
      DescriptorSetPool::Builder(*rlmDevice)
          .setMaxSets(Renderer::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(
              VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Renderer::MAX_FRAMES_IN_FLIGHT)
          .build();
  spdlog::debug("Core: Ubo descriptor set pool done");

  auto uboSet = DescriptorSet::Builder(*rlmDevice)
                    .addDescriptorSetLayout(std::move(uboDescriptorSetLayout))
                    .addDescriptorSetPool(std::move(uboDescriptorSetPool))
                    .allocateDescriptorSets()
                    .createBufferMemory(
                        sizeof(engine::component::UniformBufferObject),
                        Renderer::MAX_FRAMES_IN_FLIGHT)
                    .build();
  spdlog::debug("Core: Ubo descriptor set done");

  bool result = DescriptorSetWriter(*uboSet).writeBuffer(0).build();

  spdlog::debug("Core: UBO set fully done");
  simpleRenderSystem = std::make_unique<SimpleRenderSystem>(
      *rlmDevice,
      rlmRenderer->getRenderPass(),
      *uboSet->getDescriptorSetLayout());
  spdlog::debug("Core: SimpleRenderSystem done");
}

void Core::beginFrameOperations() {
  rlmRenderer->beginFrame();
  rlmRenderer->beginRenderPass();
}

void Core::endFrameOperations() {
  rlmRenderer->endRenderPass();
  rlmRenderer->endFrame();
  glfwPollEvents();
}

void Core::cleanup() {}
}  // namespace rlm
