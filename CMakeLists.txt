cmake_minimum_required(VERSION 3.5)

# 设置项目名称
project(Muduo)

# 设置库输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 设置 C++ 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC")

# 获取 src 目录下的所有源文件
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

# 创建共享库
add_library(Muduo SHARED ${SRC_LIST})