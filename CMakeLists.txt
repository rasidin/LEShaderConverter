cmake_minimum_required(VERSION 3.18)

project(LEShaderConverter)
enable_testing()

include(DirectXShaderCompiler.cmake)

add_executable(LEShaderConverter main.cpp "shaderconverter.h" "shaderconverter.cpp" "arguments.h" "arguments.cpp")

target_link_libraries(LEShaderConverter dxcompiler d3dcompiler)

add_test(test LEShaderConverter ${PROJECT_DIR}test.psh)