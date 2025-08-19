#pragma once

#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace renderer {
class RLMWindow {
 public:
  RLMWindow(int height, int width, const std::string &name);
  ~RLMWindow();

  bool shouldClose() { return glfwWindowShouldClose(window); }

 private:
  void initWindow();
  int height;
  int width;
  std::string windowName;

  GLFWwindow *window;
};
}  // namespace renderer
