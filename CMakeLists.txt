cmake_minimum_required(VERSION 3.18)

project(LEShaderConverter)

include(DirectXShaderCompiler.cmake)

file(GLOB FILES_CPP "*.cpp")
source_group("sources" FILES ${FILES_CPP})
file(GLOB FILES_H "*.h")
source_group("headers" FILES ${FILES_H})
file(GLOB FILES_SHADERS "shaders/*.*")
source_group("shaders" FILES ${FILES_SHADERS})

add_executable(LEShaderConverter ${FILES_CPP} ${FILES_H})

target_link_libraries(LEShaderConverter dxcompiler d3dcompiler)

file(COPY ${FILES_SHADERS} DESTINATION ${PROJECT_BINARY_DIR})

option(MAKE_SHADERCONVERTER_TEST OFF)
if (MAKE_SHADERCONVERTER_TEST)
	enable_testing()
	add_test(TestVS_5_0 LEShaderConverter -D -O0 -E=vs_main -T=VS_5_0 -N=TestVS vs5_0_test.vs vs5_0_test)
	add_test(TestPS_5_0 LEShaderConverter -D -O0 -E=ps_main -T=PS_5_0 -N=TestPS ps5_0_test.ps ps5_0_test)
	add_test(TestPS_5_0_2 LEShaderConverter -D -O0 -E=ps_main -T=PS_5_0 -N=Test2PS ps5_0_test2.ps ps5_0_test2)
	add_test(TextVS_5_0_Include LEShaderConverter -D -O0 -E=vs_main -T=VS_5_0 -N=TestVSInclude vs5_0_include_test.vs vs5_0_include_test)
endif()
