#include <fstream>
#include <string>
#include <vector>

#include "rlm_pipeline.hpp"

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace rlm {
using std::string;

RLMPipeline::RLMPipeline(
    RLMDevice &rlmDevice,
    const std::string &vertFilePath,
    const std::string &fragFilePath)
    : rlmDevice(rlmDevice) {
  createGraphicsPipeline(vertFilePath, fragFilePath);
}

RLMPipeline::~RLMPipeline() {
  vkDestroyShaderModule(rlmDevice.getDevice(), fragShaderModule, nullptr);
  vkDestroyShaderModule(rlmDevice.getDevice(), vertShaderModule, nullptr);
}

std::vector<char> RLMPipeline::readFile(const std::string &filename) {
  std::ifstream file(ENGINE_DIR + filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

void RLMPipeline::createGraphicsPipeline(const string &vertFilePath, const std::string &fragFilePath) {
  auto vertShaderCode = readFile(vertFilePath);
  auto fragShaderCode = readFile(fragFilePath);

  createShaderModule(vertShaderCode, &vertShaderModule);
  createShaderModule(fragShaderCode, &fragShaderModule);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";
  // vertShaderStageInfo.pSpecializationInfo;

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";
  // fragShaderStageInfo.pSpecializationInfo;
  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
}

void RLMPipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  auto result = vkCreateShaderModule(rlmDevice.getDevice(), &createInfo, nullptr, shaderModule);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }
}

void RLMPipeline::defaultPipelineConfigInfo(PipelineConfigInfo &configInfo) {
  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  configInfo.dynamicStateInfo.pDynamicStates = dynamicStates.data();

  configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  configInfo.viewportInfo.viewportCount = 1;
  configInfo.viewportInfo.scissorCount = 1;

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;  // Optional
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;  // Optional

  // Specifieswhat kind of geometry will be drawn from the vertices and if primitive restart should be
  // enabled. The topology member can have values like:
  //
  // -VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
  // -VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 vertices without reuse
  // -VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: the end vertex of every line is used as start vertex for the next line
  // -VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: triangle from every 3 vertices without reuse
  // -VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: the second and third vertex of every triangle are used as first
  // two vertices of the next triangle
  configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

  configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  // If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped
  // to them as opposed to discarding them. This is useful in some special cases like shadow maps. Using this
  // requires enabling a GPU feature.
  configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
  // If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage.
  // This basically disables any output to the framebuffer.
  configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
  // The polygonMode determines how fragments are generated for geometry. The following modes are available:
  //
  // -VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
  // -VK_POLYGON_MODE_LINE: polygon edges are drawn as lines
  // -VK_POLYGON_MODE_POINT: polygon vertices are drawn as points Using any mode other than fill requires
  // enabling  a GPU feature.
  configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
  // The lineWidth member is straightforward, it describes the thickness of lines in terms of number of
  // fragments. The maximum line width that is supported depends on the hardware and any line thicker
  // than 1.0f requires you to enable the wideLines GPU feature.
  // to be setup later
  configInfo.rasterizationInfo.lineWidth = 1.0f;
  // The cullMode variable determines the type of face culling to use. You can disable culling, cull the front
  // faces, cull the back faces or both. The frontFace variable specifies the vertex order for faces to be
  // considered front-facing and can be clockwise or counterclockwise.
  configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  // The rasterizer can alter the depth values by adding a constant value or biasing them based on a
  // fragment's slope. This is sometimes used for shadow mapping.
  configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
  configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
  configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
  configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

  // Configure a setting which is about multiple sampling, one of the ways to perform anti-aliasing. It works
  // by combining the fragment shader results of multiple polygons that rasterize to the same pixel. This
  // mainly occurs along edges, which is also where the most noticeable aliasing artifacts occur. Because it
  // doesn't need to run the fragment shader multiple times if only one polygon maps to a pixel, it is
  // significantly less expensive than simply rendering to a higher resolution and then downscaling. Enabling
  // it requires enabling a GPU feature.
  configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
  configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
  configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
  configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
  configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

  // This per-framebuffer struct allows you to configure the first way of color blending. The operations that
  // will be performed are best demonstrated using the following pseudocode:
  //
  // if (blendEnable) {
  //     finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor *
  //     oldColor.rgb); finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor
  //     * oldColor.a);
  // } else {
  //     finalColor = newColor;
  // }
  //
  // finalColor = finalColor & colorWriteMask;
  configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                   VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  configInfo.colorBlendAttachment.blendEnable = VK_TRUE;  // Whether to enable the operation
  configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
  configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
  configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
  configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
  configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
  configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

  configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
  configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
  configInfo.colorBlendInfo.attachmentCount = 1;
  configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
  configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
  configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
  configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
  configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

  //   VkViewport viewport{};
  //   viewport.x = 0.0f;
  //   viewport.y = 0.0f;
  //   viewport.width = (float)swapChainExtent.width;
  //   viewport.height = (float)swapChainExtent.height;
  //   viewport.minDepth = 0.0f;
  //   viewport.maxDepth = 1.0f;
  //
  //   VkRect2D scissor{};
  // scissor.offset = {0, 0};
  // scissor.extent = swapChainExtent;
}

void RLMPipeline::enableAlphaBlending(PipelineConfigInfo &configInfo) {
  // finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
  // finalColor.a = newAlpha.a;
  configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
  configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

}  // namespace rlm
