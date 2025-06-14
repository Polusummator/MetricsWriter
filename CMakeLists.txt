cmake_minimum_required(VERSION 3.24)

project(metrics-writer)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/include)

# Examples
add_executable(cpu_http_example examples/cpu_http_example.cpp)
target_link_libraries(cpu_http_example ${PROJECT_NAME})

add_executable(custom_type_example examples/custom_type_example.cpp)
target_link_libraries(custom_type_example ${PROJECT_NAME})