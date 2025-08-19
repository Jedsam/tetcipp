#pragma once

#include "rlm_window.hpp"

#include <cstdint>
#include <memory>

namespace renderer {

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// Stands for Renderer Little Machine RLM
class RLMApplication {
 public:
  void run();

  RLMApplication();
  ~RLMApplication();

 private:
  std::unique_ptr<RLMWindow> rlmWindow;
  void init();

  void mainLoop();

  void cleanup();
};

}  // namespace renderer
