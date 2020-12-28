cmake_minimum_required(VERSION 3.18)

set(DXC_BINARY_URL "https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.5.2010/dxc_2020_10-22.zip")
set(DXC_BINARY_DST ${CMAKE_BINARY_DIR}/download/dxc.zip)
if(NOT EXISTS ${DXC_BINARY_DST})
	file(DOWNLOAD ${DXC_BINARY_URL} ${DXC_BINARY_DST} SHOW_PROGRESS)
endif()

set(DXC_LIB_PATH ${CMAKE_BINARY_DIR}/depends/dxc)
if(NOT EXISTS ${DXC_LIB_PATH})
	file(MAKE_DIRECTORY ${DXC_LIB_PATH})
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf ${DXC_BINARY_DST}
		WORKING_DIRECTORY ${DXC_LIB_PATH}
	)
endif()

include_directories(${DXC_LIB_PATH}/inc)
link_directories(${DXC_LIB_PATH}/lib/${CMAKE_GENERATOR_PLATFORM})