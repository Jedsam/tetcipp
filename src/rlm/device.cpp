
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

// logger
#include "spdlog/spdlog.h"

#include "device.hpp"
#include "window.hpp"

namespace rlm {

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {

  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  //   if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
  //     // Message is important enough to show
  // }

  return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

Device::Device(RLMWindow &window) : rlmWindow{window} {
  createInstance();
  spdlog::debug("RLMDevice: Instance created\n");
  setupDebugMessenger();
  spdlog::debug("RLMDevice: Debug messenger done\n");
  createSurface();
  spdlog::debug("RLMDevice: Surface created\n");
  pickPhysicalDevice();
  spdlog::debug("RLMDevice: physical devices picked\n");
  createLogicalDevice();
  spdlog::debug("RLMDevice: Logical device created\n");
  createCommandPool();
  spdlog::debug("RLMDevice: Command pool created\n");
}

Device::~Device() {
  vkDestroyCommandPool(device, commandPool, nullptr);
  // Destroy them in reverse order they were created
  vkDestroyDevice(device, nullptr);

  if (enableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }

  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);
}

void Device::createCommandPool() {
  QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

  VkCommandPoolCreateInfo poolInfo{};
  // There are two possible flags for command pools:

  // -VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers are
  // rerecorded with new commands very often (may change memory allocation
  // behavior) -VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow command
  // buffers to be rerecorded individually, without this flag they all have to
  // be reset together
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
  auto result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void Device::createInstance() {
  if (enableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("Validation layers requested, but not available!");
  }

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  // appInfo.pNext = nullptr;
  appInfo.pApplicationName = "RendererLittleMachine Application";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto glfwExtensions = getRequiredExtensions();

  createInfo.enabledExtensionCount = glfwExtensions.size();
  createInfo.ppEnabledExtensionNames = glfwExtensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (enableValidationLayers) {
    std::cout << std::format("The instance is in debug mode\n");

    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT *>(
        &debugCreateInfo);
  } else {
    createInfo.enabledLayerCount = 0;
  }

  auto result = vkCreateInstance(&createInfo, nullptr, &instance);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create instance!");
  }

  checkGLFWHasRequiredExtensions(glfwExtensions);
}

void Device::setupDebugMessenger() {
  if (!enableValidationLayers)
    return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  populateDebugMessengerCreateInfo(createInfo);

  auto result = CreateDebugUtilsMessengerEXT(
      instance, &createInfo, nullptr, &debugMessenger);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to set up debug messenger");
  }
}

void Device::createSurface() {
  auto result = glfwCreateWindowSurface(
      instance, rlmWindow.getGLFWWindow(), nullptr, &surface);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void Device::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

  // Use an ordered map to automatically sort candidates by increasing score
  std::multimap<int, VkPhysicalDevice> candidates;

  spdlog::debug("RLMDevice::pickPhysicalDevice: About to rate devices!");
  for (const auto &myDevice : physicalDevices) {
    int score = rateDeviceSuitability(myDevice);
    candidates.insert(std::make_pair(score, myDevice));
  }
  spdlog::debug(
      "RLMDevice::pickPhysicalDevice: Rated all devices succesfully!");

  // Check if the best candidate is suitable at all
  if (candidates.rbegin()->first > 0) {
    physicalDevice = candidates.rbegin()->second;
  } else {
    throw std::runtime_error("failed to find a suitable GPU!");
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }

  vkGetPhysicalDeviceProperties(physicalDevice, &properties);
  std::cout << "physical device: " << properties.deviceName << std::endl;
}

void Device::createLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {
      indices.graphicsFamily.value(), indices.presentFamily.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  auto result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }

  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

QueueFamilyIndices
Device::findQueueFamilies(VkPhysicalDevice myPhysicalDevice) {
  spdlog::debug("RLMDevice::findQueueFamilies: Inside the function");
  QueueFamilyIndices indices;
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(
      myPhysicalDevice, &queueFamilyCount, nullptr);
  spdlog::debug("RLMDevice::findQueueFamilies: Got the family count");

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      myPhysicalDevice, &queueFamilyCount, queueFamilies.data());
  spdlog::debug("RLMDevice::findQueueFamilies: Got the queue family data");

  uint32_t i = 0;
  for (auto queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        myPhysicalDevice, i, surface, &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }
    if (indices.isComplete())
      break;
    i++;
  }

  return indices;
}

int Device::rateDeviceSuitability(VkPhysicalDevice myPhysicalDevice) {
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(myPhysicalDevice, &deviceProperties);
  vkGetPhysicalDeviceFeatures(myPhysicalDevice, &deviceFeatures);

  int score = 0;

  // Discrete GPUs have a significant performance advantage
  if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  // Maximum possible size of textures affects graphics quality
  score += deviceProperties.limits.maxImageDimension2D;

  // Application can't function without geometry shaders
  if (!(deviceFeatures.geometryShader && isDeviceSuitable(myPhysicalDevice))) {
    return 0;
  }

  return score;
}

bool Device::isDeviceSuitable(VkPhysicalDevice myPhysicalDevice) {
  spdlog::debug("RLMDevice::isDeviceSuitable: step 1");
  QueueFamilyIndices indices = findQueueFamilies(myPhysicalDevice);
  if (!indices.isComplete())
    return false;
  spdlog::debug("RLMDevice::isDeviceSuitable: step 2");

  if (!checkDeviceExtensionSupport(myPhysicalDevice))
    return false;

  spdlog::debug("RLMDevice::isDeviceSuitable: step 3");
  SwapChainSupportDetails swapChainSupport =
      querySwapChainSupport(myPhysicalDevice);
  if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
    return false;

  spdlog::debug("RLMDevice::isDeviceSuitable: step 4");
  return true;
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice myPhysicalDevice) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(
      myPhysicalDevice, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(
      myPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

  std::set<std::string_view> requiredExtensions(
      deviceExtensions.begin(), deviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

SwapChainSupportDetails
Device::querySwapChainSupport(VkPhysicalDevice myPhysicalDevice) {
  SwapChainSupportDetails details;

  auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      myPhysicalDevice, surface, &details.capabilities);
  if (result != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to get physical device surface capabilities");
  }

  uint32_t formatCount;
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(
      myPhysicalDevice, surface, &formatCount, nullptr);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to get physical device surface formats");
  }

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(
        myPhysicalDevice, surface, &formatCount, details.formats.data());
    if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to get physical device surface formats");
    }
  }

  uint32_t presentModeCount;
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(
      myPhysicalDevice, surface, &presentModeCount, nullptr);
  if (result != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to get physical device surface present modes");
  }

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(
        myPhysicalDevice,
        surface,
        &presentModeCount,
        details.presentModes.data());
    if (result != VK_SUCCESS) {
      throw std::runtime_error(
          "failed to get physical device surface present modes");
    }
  }

  return details;
}

void Device::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
}

void Device::checkGLFWHasRequiredExtensions(
    const std::vector<const char *> &requiredExtensions) {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(
      nullptr, &extensionCount, extensions.data());

  std::cout << "available extensions:\n";
  std::unordered_set<std::string> available;
  for (const auto &extension : extensions) {
    std::cout << '\t' << extension.extensionName << '\n';
    available.insert(extension.extensionName);
  }
  for (const auto &required : requiredExtensions) {
    std::cout << "\t" << required << std::endl;
    if (available.find(required) == available.end()) {
      throw std::runtime_error("Missing required glfw extension");
    }
  }
}

std::vector<const char *> Device::getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(
      glfwExtensions, glfwExtensions + glfwExtensionCount);

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

bool Device::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : validationLayers) {
    bool layerFound = std::any_of(
        availableLayers.begin(),
        availableLayers.end(),
        [&layerName](const auto &layerProperties) {
          return strcmp(layerName, layerProperties.layerName) == 0;
        });

    if (!layerFound) {
      return false;
    }
  }

  return true;
}

void Device::createBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer &buffer,
    VkDeviceMemory &bufferMemory) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create vertex buffer!");
  }

  // The VkMemoryRequirements struct has three fields:
  //
  // size: The size of the required amount of memory in bytes, may differ from
  // bufferInfo.size. alignment: The offset in bytes where the buffer begins in
  // the allocated region of memory, depends on bufferInfo.usage and
  // bufferInfo.flags. memoryTypeBits: Bit field of the memory types that are
  // suitable for the buffer.
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(memRequirements.memoryTypeBits, properties);

  auto result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate vertex buffer memory!");
  }

  // The first three parameters are self-explanatory and the fourth parameter is
  // the offset within the region of memory. Since this memory is allocated
  // specifically for this the vertex buffer, the offset is simply 0. If the
  // offset is non-zero, then it is required to be divisible by
  // memRequirements.alignment.
  vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Device::copyBuffer(
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size) {
  // NOTE TO : The implementation may be able to apply memory allocation
  // optimizations. VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag should be used
  // during command pool generation in that case.
  VkCommandBuffer commandBuffer = beginSingleTimeCommand();

  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0;  // Optional
  copyRegion.dstOffset = 0;  // Optional
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  endSingleTimeCommand(commandBuffer);
}

VkCommandBuffer Device::beginSingleTimeCommand() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void Device::endSingleTimeCommand(VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphicsQueue);

  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

uint32_t
Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

}  // namespace rlm
