# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("AnEngine.OutputTarget.Window")

set_property(GLOBAL PROPERTY USE_FOLDERS On)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (WIN32)
	add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
	add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>)
endif(WIN32)

file(GLOB_RECURSE SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE INC "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

include_directories(${AnEngine_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR})

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "*.*")

# add_executable(AnEngine.OutputTarget.Window ./WinMain.cpp)
add_executable(AnEngine.OutputTarget.Window WIN32 ${SRC} ${INC})

target_link_libraries(AnEngine.OutputTarget.Window "AnEngine")

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _UNICODE UNICODE _WIN32_WINNT=0x0A00)
endif(WIN32)

if(MSVC)
    if (${CMAKE_SIZEOF_VOID_P} EQUAL "4" AND (NOT ${CMAKE_VS_PLATFORM_NAME} MATCHES "arm"))
        target_compile_options(${PROJECT_NAME} PRIVATE /arch:SSE2)
    endif()
endif()