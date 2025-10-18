#include <spdlog/spdlog-inl.h>
#include <vulkan/vulkan.h>

#include <cstdlib>
#include <iostream>

#include "Game.hpp"
#include "spdlog/sinks/basic_file_sink.h"

namespace app {

void init_logger() {
  try {
    auto logger =
        spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt", true);
    spdlog::set_default_logger(logger);

#ifdef NDEBUG
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_every(std::chrono::seconds(10));
#else
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
#endif

    // Optional formatting
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    spdlog::info("Logger initialized");
  } catch (const spdlog::spdlog_ex &ex) {
    std::cout << "Log init failed: " << ex.what() << std::endl;
  }
}

int main() {
  // Initialize logger
  init_logger();

  Game app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
}  // namespace app
