#pragma once

#include <memory>

#include "rlm/model.hpp"

namespace engine::component {

struct ModelComponent {
  std::shared_ptr<rlm::Model> model;
};
}  // namespace engine::component
