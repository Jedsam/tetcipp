#pragma once

#include "rlm_device.hpp"
#include "rlm_window.hpp"

#include <cstdint>
#include <memory>

namespace rlm {

const uint32_t WIDTH = 1200;
const uint32_t HEIGHT = 900;

// Stands for Renderer Little Machine RLM
class RLMApplication {
 public:
  void run();

  RLMApplication();
  ~RLMApplication();

 private:
  std::unique_ptr<RLMWindow> window;
  std::unique_ptr<RLMDevice> device;
  void init();

  void mainLoop();

  void cleanup();
};

}  // namespace rlm
