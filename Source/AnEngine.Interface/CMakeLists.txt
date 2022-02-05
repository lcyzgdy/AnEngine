# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("AnEngine.Interface")

set_property(GLOBAL PROPERTY USE_FOLDERS On)

file(GLOB_RECURSE INC "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

include_directories(${PROJECT_SOURCE_DIR})

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "*.*")

add_library(${PROJECT_NAME} INTERFACE ${INC})