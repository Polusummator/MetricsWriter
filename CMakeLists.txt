cmake_minimum_required(VERSION 3.24)

project(metrics-writer)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(queue
    GIT_REPOSITORY https://github.com/cameron314/concurrentqueue
)
FetchContent_MakeAvailable(queue)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(metrics-writer INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_include_directories(${PROJECT_NAME} INTERFACE ${queue_SOURCE_DIR})

# Examples
add_executable(cpu_http_example examples/cpu_http_example.cpp)
target_link_libraries(cpu_http_example ${PROJECT_NAME})

add_executable(custom_type_example examples/custom_type_example.cpp)
target_link_libraries(custom_type_example ${PROJECT_NAME})

add_executable(latency_example examples/latency_example.cpp)
target_link_libraries(latency_example ${PROJECT_NAME})

add_executable(custom_aggregator_example examples/custom_aggregator_example.cpp)
target_link_libraries(custom_aggregator_example ${PROJECT_NAME})