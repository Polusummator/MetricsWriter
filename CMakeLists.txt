cmake_minimum_required(VERSION 3.24)

project(metrics-writer)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC_LIST src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)