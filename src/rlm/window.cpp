
#include <GLFW/glfw3.h>

#include <string>

#include "window.hpp"

namespace rlm {
Window::Window(int height, int width, const std::string &name)
    : height{height}, width{width}, windowName{name} {
  initWindow();
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Window::initWindow() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window =
      glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void Window::framebufferResizeCallback(
    GLFWwindow *window,
    int width,
    int height) {
  auto rlmWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  rlmWindow->framebufferResized = true;
  rlmWindow->width = width;
  rlmWindow->height = height;
}
}  // namespace rlm
