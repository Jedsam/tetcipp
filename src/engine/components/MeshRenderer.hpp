#pragma once

#include <memory>

#include "rlm/rlm_model.hpp"

namespace component {

struct ModelComponent {
  std::shared_ptr<rlm::RLMModel> model;
};
}  // namespace component
