#pragma once

#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace rlm {
class Window {
 public:
  Window(int height, int width, const std::string &name);
  ~Window();

  bool shouldClose() { return glfwWindowShouldClose(window); }

  GLFWwindow *getGLFWWindow() { return window; }

  VkExtent2D getExtent() {
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }

  bool wasWindowResized() { return framebufferResized; }

  void resetWindowResizedFlag() { framebufferResized = false; }

 private:
  static void
  framebufferResizeCallback(GLFWwindow *window, int width, int height);

  void initWindow();
  int height;
  int width;
  std::string windowName;

  bool framebufferResized = false;

  GLFWwindow *window;
};
}  // namespace rlm
