cmake_minimum_required(VERSION 3.10)
project(GraphicsProject LANGUAGES C CXX)

# --- C++ standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Silence the GLVND warning (optional but recommended) ---
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()
set(OpenGL_GL_PREFERENCE GLVND)

# --- Your own headers ---
include_directories(${PROJECT_SOURCE_DIR}/include)

# --- Find OpenGL (OK to use system OpenGL) ---
find_package(OpenGL REQUIRED)

# --- Build the vendored GLFW you cloned into ./GLFW (no sudo needed) ---
# Make sure you have: Robot_Graphic/GLFW/{CMakeLists.txt, include/, src/ ...}
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/GLFW)

# --- Sources (add everything you actually use) ---
set(SOURCES
    src/main.cpp
    src/glad.c
    src/shader.cpp
    src/cube.cpp
    src/robot.cpp
    src/scene.cpp
    src/camera.cpp
)

# --- Executable ---
add_executable(graphics_program ${SOURCES})

# --- Include paths (GLFW headers + your include/) ---
target_include_directories(graphics_program PRIVATE
    ${CMAKE_SOURCE_DIR}/GLFW/include
    ${PROJECT_SOURCE_DIR}/include
)

# --- Link libraries ---
target_link_libraries(graphics_program PRIVATE
    glfw
    OpenGL::GL
    ${CMAKE_DL_LIBS}
)

# --- Platform specifics ---
if(UNIX AND NOT APPLE)
    # GLFW on Linux typically needs these extra system libs
    target_link_libraries(graphics_program PRIVATE
        pthread
        X11 Xrandr Xi Xxf86vm Xinerama Xcursor
        dl
    )
elseif(APPLE)
    target_link_libraries(graphics_program PRIVATE
        "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
    )
endif()
