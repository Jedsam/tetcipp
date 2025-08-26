
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "renderer/rlm_window.hpp"
#include "rlm_device.hpp"

namespace rlm {

RLMDevice::RLMDevice(RLMWindow &window) : window{window} { createInstance(); }

RLMDevice::~RLMDevice() { vkDestroyInstance(instance, nullptr); }

void RLMDevice::createInstance() {
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

  createInfo.enabledLayerCount = 0;

  auto result = vkCreateInstance(&createInfo, nullptr, &instance);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }

  checkGLFWHasRequiredExtensions(glfwExtensions);
}

void RLMDevice::checkGLFWHasRequiredExtensions(const std::vector<const char *> &requiredExtensions) {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

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

std::vector<const char *> RLMDevice::getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

}  // namespace rlm
