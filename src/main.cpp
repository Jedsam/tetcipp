#include <vulkan/vulkan.h>

#include <cstdlib>
#include <iostream>

#include "renderer/rlm_tetcipp_app.hpp"

int main() {
  renderer::RLMApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
