#pragma once

#include <atomic>

namespace ecs {
class IDGenerator {
 public:
  uint32_t nextID() { return current_id.fetch_add(1); }

 private:
  std::atomic<uint32_t> current_id = 1;
};
}  // namespace ecs
