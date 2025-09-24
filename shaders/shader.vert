#version 460

// Just like fragColor, the layout(location = x) annotations assign indices to the inputs that we can later
// use to reference them. It is important to know that some types, like dvec3 64 bit vectors, use multiple
// slots. That means that the index after it must be at least 2 higher

// clang-format off
layout(binding = 0) uniform UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;
// clang-format on

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
  fragColor = inColor;
}
