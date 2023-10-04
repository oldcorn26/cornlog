cmake_minimum_required(VERSION 3.12)  # 从3.12版本开始支持C++20

project(MyLogProject)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加源文件
set(SOURCE_FILES
        src/file_writer.cpp
        src/log_file_appender.cpp
        main.cpp
)

# 生成可执行文件
add_executable(mylog ${SOURCE_FILES})

target_link_libraries(mylog pthread)
