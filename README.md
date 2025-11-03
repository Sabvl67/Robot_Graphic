# OpenGL Graphics Project

A cross-platform OpenGL graphics project using GLFW and GLAD, compatible with both macOS and Linux.

## Project Structure

```
CompGraph_P/
├── include/
│   ├── glad/
│   │   └── glad.h
│   ├── GLFW/
│   │   ├── glfw3.h
│   │   └── glfw3native.h
│   └── KHR/
│       └── khrplatform.h
├── src/
│   ├── main.cpp          # Main program
│   └── glad.c            # GLAD OpenGL loader
├── Makefile              # For make-based builds
├── CMakeLists.txt        # For CMake-based builds
└── README.md
```

## Prerequisites

### Linux (Comp Lab)

Install required packages:

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential libglfw3-dev libgl1-mesa-dev

# Fedora/RHEL
sudo dnf install gcc-c++ glfw-devel mesa-libGL-devel

# Arch Linux
sudo pacman -S base-devel glfw-x11 mesa
```

### macOS

Install GLFW via Homebrew:

```bash
brew install glfw
```

## Building the Project

### Option 1: Using Make (Recommended for Linux)

```bash
# Build the project
make

# Run the program
./graphics_program

# Or build and run in one command
make run

# Clean build artifacts
make clean

# Rebuild from scratch
make rebuild
```

### Option 2: Using CMake

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Run the program
./graphics_program
```

## Troubleshooting

### Linux Comp Lab Issues

**Problem: GLFW not found**
```bash
# Check if GLFW is installed
pkg-config --modversion glfw3

# If not installed, contact lab administrator or install locally
```

**Problem: OpenGL driver issues**
```bash
# Check OpenGL version
glxinfo | grep "OpenGL version"

# Update graphics drivers if needed
```

**Problem: Missing libraries**
```bash
# Check what libraries the executable needs
ldd graphics_program

# Install missing dependencies
```

### macOS Issues

**Problem: Framework not found**
- Make sure Xcode Command Line Tools are installed:
```bash
xcode-select --install
```

**Problem: GLFW not found**
- Install via Homebrew:
```bash
brew install glfw
```

## Running on Different Systems

### Linux Comp Lab & macOS

The Makefile automatically detects Linux and uses the correct linking flags. Simply:

```bash
make
./graphics_program
```

## Development

### Adding New Source Files

1. Add `.cpp` files to the `src/` directory
2. Update the `SOURCES` variable in `Makefile`:
   ```makefile
   SOURCES = src/main.cpp src/glad.c src/your_file.cpp
   ```
3. For CMake, update `CMakeLists.txt`:
   ```cmake
   set(SOURCES
       src/main.cpp
       src/glad.c
       src/abc.cpp
   )
   ```

### Adding Header Files

Place custom header files in the `include/` directory. They will be automatically included.

## OpenGL Version

This project is configured for OpenGL 3.3 Core Profile, which is widely supported. You can modify the version in [src/main.cpp](src/main.cpp):

```cpp
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
```

## Controls

- **ESC**: Close the window

## Notes for Comp Lab Submission

- The project includes all necessary headers (GLAD, GLFW)
- Only GLFW library needs to be installed on the system
- The Makefile works with standard `g++` compiler
- No special permissions or dependencies required

## Resources

- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [OpenGL Reference](https://www.khronos.org/opengl/wiki/)
- [Learn OpenGL](https://learnopengl.com/)

## License

Educational project for computer graphics coursework.
