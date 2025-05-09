cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.c)

# Add your source files
add_executable(game ${SRC_FILES})

# Include directories
target_include_directories(game PRIVATE headers headers/imgui include)

# Link libraries
target_link_libraries(game PRIVATE OpenGL::GL glfw dl)

# Copy resources
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
