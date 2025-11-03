cmake_minimum_required(VERSION 3.10)
project(GraphicsProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(glfw3 3.3 REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/glad.c
)

# Create executable
add_executable(graphics_program ${SOURCES})

# Link libraries
target_link_libraries(graphics_program
    OpenGL::GL
    glfw
    ${CMAKE_DL_LIBS}
)

# Platform-specific settings
if(UNIX AND NOT APPLE)
    target_link_libraries(graphics_program pthread)
endif()

if(APPLE)
    target_link_libraries(graphics_program
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()
