#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

#include "renderer/rlm_window.hpp"

namespace rlm {
class RLMDevice {
 public:
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif

  explicit RLMDevice(RLMWindow &window);
  ~RLMDevice();

 private:
  void createInstance();

  void checkGLFWHasRequiredExtensions(const std::vector<const char *> &requiredExtensions);

  std::vector<const char *> getRequiredExtensions();

  RLMWindow &window;
  VkInstance instance;
};
}  // namespace rlm
