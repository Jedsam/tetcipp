#pragma once

#include <memory>

#include "rlm/model.hpp"

namespace engine::component {

struct ModelComponent {
  std::unique_ptr<rlm::Model> model;
};
}  // namespace engine::component
