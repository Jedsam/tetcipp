#pragma once

#include <memory>

#include "renderer/rlm_swapchain.hpp"
#include "renderer/rlm_window.hpp"

namespace rlm {
class RLMRenderer {
 public:
  RLMRenderer(RLMWindow &rlmWindow, RLMDevice &rlmDevice);
  ~RLMRenderer();

 private:
  RLMDevice &rlmDevice;
  RLMWindow &rlmWindow;
  std::unique_ptr<RLMSwapChain> rlmSwapChain;
  void recreateSwapChain();
};
}  // namespace rlm
