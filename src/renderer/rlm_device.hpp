#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>
#include <vector>

#include "renderer/rlm_window.hpp"

namespace rlm {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;

  bool isComplete() { return graphicsFamily.has_value(); }
};

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
  /// Creates the Vulkan instance used by this device.
  /// Must be called before creating any Vulkan resources.
  /// @throws std::runtime_error if instance creation fails.
  void createInstance();

  /// Setups the way debug messages are handled and creates the required struct
  /// @throws std::runtime_error if instance creation fails.
  void setupDebugMessenger();

  /// @throws std::runtime_error if instance creation fails.
  void pickPhysicalDevice();

  /// Template VkDebugUtilsMessengerCreateInfoEXT struct to be reused
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  /// Verifies that GLFW supports all required Vulkan extensions.
  /// @param requiredExtensions List of required Vulkan extension names.
  /// @throws std::runtime_error if one or more required extensions are missing.
  void checkGLFWHasRequiredExtensions(const std::vector<const char *> &requiredExtensions);

  /// Returns the list of Vulkan instance extensions required by GLFW.
  /// @return A vector of extension name strings.
  std::vector<const char *> getRequiredExtensions();

  /// Checks if all validation layers exist in the available layers
  /// @return true for no problem
  bool checkValidationLayerSupport();

  int rateDeviceSuitability(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  RLMWindow &window;

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
};
}  // namespace rlm
