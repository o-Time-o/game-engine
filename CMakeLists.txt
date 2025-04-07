cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(app
    src/main.cpp 
    src/glad.c 
    src/Shader.cpp 
    src/VAO.cpp 
    src/VBO.cpp 
    src/EBO.cpp
	src/stb.cpp
)

target_include_directories(app PRIVATE headers include)

target_link_libraries(app PRIVATE OpenGL::GL glfw)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
