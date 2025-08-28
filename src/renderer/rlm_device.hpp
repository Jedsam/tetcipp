#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>
#include <vector>

#include "renderer/rlm_window.hpp"

namespace rlm {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class RLMDevice {
 public:
#ifdef NDEBUG
  static const bool enableValidationLayers = false;
#else
  static const bool enableValidationLayers = true;
#endif

  SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }

  VkSurfaceKHR getSurface() { return surface; }

  QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }

  VkDevice getDevice() { return device; }

  explicit RLMDevice(RLMWindow &window);
  ~RLMDevice();

 private:
  /// Creates the Vulkan instance used by this device.
  /// Must be called before creating any Vulkan resources.
  /// @throws std::runtime_error if instance creation fails.
  void createInstance();

  /// Setups the way debug messages are handled and creates the required struct
  /// @throws std::runtime_error if debug messenger creation fails.
  void setupDebugMessenger();

  void createSurface();

  /// Function to choose the most optimal physical device to work with
  /// @throws std::runtime_error if no physical device found.
  void pickPhysicalDevice();

  /// Function to create a logical device based on physical device which also holds queue information
  /// @throws std::runtime_error if instance creation fails.
  void createLogicalDevice();

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

  /// Checks properties, features and queue families of a given device to return a suitabilty score
  /// @return An int score based on the suitablity of the device, 0 for incompatible devices
  int rateDeviceSuitability(VkPhysicalDevice myPhysicalDevice);

  ///
  bool isDeviceSuitable(VkPhysicalDevice myPhysicalDevice);

  /// Finds the queue family indices of a given physical device
  /// @return A struct that holds the family indicies
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice myPhysicalDevice);

  ///
  bool checkDeviceExtensionSupport(VkPhysicalDevice myPhysicalDevice);

  ///
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice myPhysicalDevice);

  RLMWindow &rlmWindow;

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;
  VkDevice device;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkPhysicalDeviceProperties properties;

  VkQueue graphicsQueue;
  VkQueue presentQueue;

  const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
  const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
}  // namespace rlm
