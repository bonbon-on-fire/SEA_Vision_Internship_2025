cmake_minimum_required(VERSION 3.15)
project(sea_vision_project)

# c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Load conan dependencies for nlohmann/json
find_package(nlohmann_json REQUIRED)

# Use pre-built OpenCV
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/opencv/build")
find_package(OpenCV REQUIRED)

# executable
add_executable(sea_vision
    main.cpp
    src/cpp/operations/cpp/base_operation.cpp
    src/cpp/operations/cpp/operations.cpp
    src/cpp/bindings/cpp/pipeline_reader.cpp
    src/cpp/bindings/cpp/operation_factory.cpp
    src/cpp/graph/cpp/graph_node.cpp
    src/cpp/graph/cpp/graph.cpp
    src/cpp/graph/cpp/graph_node_factory.cpp
    src/cpp/graph/cpp/graph_executor.cpp
)

# link libraries
target_link_libraries(sea_vision
    ${OpenCV_LIBS}
    nlohmann_json::nlohmann_json
)

# include directories
target_include_directories(sea_vision PRIVATE 
    src/cpp
)

# output directory
set_target_properties(sea_vision PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)