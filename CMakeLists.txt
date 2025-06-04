cmake_minimum_required(VERSION 3.0)
project(mrpc)

set(CMAKE_BUILD_TYPE "Debug")


# 项目可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 项目库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# 项目编译头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
# 项目库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)


# src包含了mprpc框架所有的相关代码
add_subdirectory(src)

# example包含了mprpc框架使用的实例代码
add_subdirectory(example)