# OpenGL Graphics Project

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


```
Base (root)
 └─ Torso (translate up from base)
     ├─ Head (translate up from torso, rotate on Y-axis)
     ├─ Left Arm (translate left from torso, rotate on Z-axis for lifting)
     │   └─ Left Hand (translate from arm end)
     └─ Right Arm (translate right from torso, rotate on Z-axis)
         └─ Right Hand (translate from arm end)
```

## Prerequisites

## Building the Project

### Using CMake

```bash
# Build the project
cmake -B build && cmake --build build && cmake --build build --target run

#Shorthand to compile and launch
cmake -B build && cmake --build build && ./build/graphics_program

```
### 🧩 Dependencies

- **GLFW** – included directly in the repo under `GLFW/` (not system-installed).  
  CMake automatically builds it during compilation — no external setup needed.  
- **OpenGL** – uses system’s default OpenGL library.  

## Development

### Adding New Source Files

1. Add `.cpp` files to the `src/` directory
2. Update the `SOURCES` variable in `Makefile`:
   ```makefiyourle
   SOURCES = src/main.cpp src/glad.c src/file.cpp
   ```
3. For CMake, update `CMakeLists.txt`:
   ```cmake
   set(SOURCES
       src/main.cpp
       src/glad.c
       src/abc.cpp
   )
   ```

## OpenGL Version

This project is configured for OpenGL 3.3 Core Profile, modify the version in [src/main.cpp](src/main.cpp):

## Notes for Comp Lab Submission

- The project includes all necessary headers (GLAD, GLFW)
- Only GLFW library needs to be installed on the system
- The Makefile works with standard `g++` compiler
- No special permissions or dependencies required

## Resources

- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [OpenGL Reference](https://www.khronos.org/opengl/wiki/)
- [Learn OpenGL](https://learnopengl.com/)
