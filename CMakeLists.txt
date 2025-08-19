cmake_minimum_required(VERSION 3.10)
project(vulkanproject CXX)
set(CMAKE_CXX_STANDARD 23)

# necessary for clangd to understand the file structure
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)
find_package(assimp REQUIRED)
find_package(glfw3 REQUIRED)

set(EXECUTABLES main)
# Get the cpp files needed
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
set(sources_main ${SOURCES})
# Add hpp files folder

foreach(executable ${EXECUTABLES})
  add_executable(${executable} ${sources_${executable}} ${HELPING_FUNCTIONS})
  target_include_directories(${executable} PUBLIC ${PROJECT_SOURCE_DIR}/src)
  target_link_libraries(${executable} PRIVATE Vulkan::Vulkan glfw assimp)
endforeach()

find_program(
  GLSL_VALIDATOR
  glslangValidator
  HINTS
    ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
    /usr/bin
    /usr/local/bin
    ${VULKAN_SDK_PATH}/Bin
    ${VULKAN_SDK_PATH}/Bin32
    $ENV{VULKAN_SDK}/Bin/
    $ENV{VULKAN_SDK}/Bin32/
)

# get all .vert and .frag files in shaders directory
file(
  GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/build/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL}
  )
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})
