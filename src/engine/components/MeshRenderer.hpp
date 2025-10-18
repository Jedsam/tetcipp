#pragma once

#include <memory>

#include "rlm/model.hpp"

namespace component {

struct ModelComponent {
  std::shared_ptr<rlm::Model> model;
};
}  // namespace component
