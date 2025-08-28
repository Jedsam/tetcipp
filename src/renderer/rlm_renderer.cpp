
#include <memory>

#include "renderer/rlm_device.hpp"
#include "renderer/rlm_renderer.hpp"
#include "renderer/rlm_swapchain.hpp"
#include "renderer/rlm_window.hpp"

namespace rlm {

RLMRenderer::RLMRenderer(RLMWindow &rlmWindow, RLMDevice &rlmDevice)
    : rlmWindow(rlmWindow), rlmDevice(rlmDevice) {
  recreateSwapChain();
}

RLMRenderer::~RLMRenderer() {}

void RLMRenderer::recreateSwapChain() {
  auto extent = rlmWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = rlmWindow.getExtent();
    glfwWaitEvents();
  }
  rlmSwapChain = std::make_unique<RLMSwapChain>(rlmDevice);
}
}  // namespace rlm
