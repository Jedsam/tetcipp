#pragma once

#include "rlm/tetcipp_app.hpp"

namespace engine {
class Engine {
 public:
  Engine() : myApplication() {}

  rlm::Device &getDevice() { return myApplication.getDevice(); }

 private:
  rlm::Application myApplication;
};
}  // namespace engine
