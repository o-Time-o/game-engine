cmake_minimum_required(VERSION 3.10)
project(Game)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

file(GLOB IMGUI_SRC imgui/*.cpp)
list(APPEND SRC_FILES ${IMGUI_SRC})

# Add your source files
add_executable(game ${SRC_FILES})

# Include directories
target_include_directories(game PRIVATE
	include
	headers
	imgui
)

# Link libraries
target_link_libraries(game PRIVATE
	${CMAKE_SOURCE_DIR}/lib/libbox2d.a
	${CMAKE_SOURCE_DIR}/lib/libsfml-graphics-s.a
    ${CMAKE_SOURCE_DIR}/lib/libsfml-window-s.a
    ${CMAKE_SOURCE_DIR}/lib/libsfml-system-s.a
    ${CMAKE_SOURCE_DIR}/lib/libsfml-network-s.a
    ${CMAKE_SOURCE_DIR}/lib/libsfml-audio-s.a

	freetype
    jpeg
    openal
    vorbis
    vorbisfile
	FLAC
    X11
	Xi
	Xcursor
    Xrandr
    GL
    udev
    dl
    pthread
)

set_target_properties(game PROPERTIES
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib"
)

# Copy resources
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
