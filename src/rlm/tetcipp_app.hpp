#pragma once

#include <cstdint>
#include <memory>

#include "device.hpp"
#include "renderer.hpp"
#include "window.hpp"

// Stands for Renderer Little Machine
namespace rlm {

const uint32_t WIDTH = 1200;
const uint32_t HEIGHT = 900;

class Application {
 public:
  void run();

  Application();
  ~Application();

  Device &getDevice() { return *rlmDevice; }

 private:
  std::unique_ptr<Window> rlmWindow;
  std::unique_ptr<Device> rlmDevice;
  std::unique_ptr<Renderer> rlmRenderer;
  void init();

  void mainLoop();

  void cleanup();
};

}  // namespace rlm
