# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("AnEngine.All")

set_property(GLOBAL PROPERTY USE_FOLDERS On)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Build/bin) # 指定可执行文件输出目录

# 包含子项目。
add_subdirectory("./Source/3rdparty")
add_subdirectory("./Source/AnEngine")
