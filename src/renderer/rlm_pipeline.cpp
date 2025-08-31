#include <string>

#include "rlm_pipeline.hpp"

namespace rlm {
using std::string;

RLMPipeline::RLMPipeline(
    RLMDevice &rlmDevice,
    const std::string &vertFilePath,
    const std::string &fragFilePath)
    : rlmDevice(rlmDevice) {
  createGraphicsPipeline(vertFilePath, fragFilePath);
}

RLMPipeline::~RLMPipeline() {}

void RLMPipeline::createGraphicsPipeline(const string &vertFilePath, const std::string &fragFilePath) {}
}  // namespace rlm
