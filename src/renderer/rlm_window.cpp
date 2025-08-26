
#include <GLFW/glfw3.h>

#include <string>

#include "rlm_window.hpp"

namespace rlm {
RLMWindow::RLMWindow(int height, int width, const std::string &name)
    : height{height}, width{width}, windowName{name} {
  initWindow();
}

RLMWindow::~RLMWindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void RLMWindow::initWindow() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(height, width, windowName.c_str(), nullptr, nullptr);
}
}  // namespace rlm
