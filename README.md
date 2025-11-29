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

## Building the Project

### Using CMake

```bash

# Compile and launch
cmake -B build && cmake --build build && ./build/graphics_program

# Build the project
cmake -B build && cmake --build build && cmake --build build --target run

```

## Controls

### Scene Selection
- **`1`** - Day Scene (bright daylight with white-yellow sun)
- **`2`** - Night Scene (dark with cool blue moonlight)
- **`3`** - Sunset Scene (warm orange-red lighting)

### Camera Modes
- **`O`** - Orbit Camera (rotates around robot automatically)
- **`I`** - Static Front Camera (fixed front view)
- **`U`** - Free Camera (WASD to move, Arrow Keys to look, Q/E for up/down)

**Note:** In Free Camera mode (U), manual joint controls are disabled to avoid key conflicts.

### Robot Animations
**Automatic Animations (Toggle ON/OFF):**
- **`SPACE`** - Toggle idle walk animation (leg walking motion)
- **`W`** - Toggle arm wave animation (both arms wave up and down)
- **`B`** - Toggle head bobbing animation (gentle nod)
- **`T`** - Toggle torso sway animation (gentle body rotation)

**Manual Controls:**
- **`P`** - Trigger one-step animation (single step sequence)
- **`R`** - Reset robot pose (all joints to neutral, stop all animations)

### Manual Joint Control
*(Not available in Free Camera mode)*
**Head:**
- `Q` / `E` - Rotate head left/right

**Left Arm:**
- `A` / `S` - Shoulder rotation
- `Z` / `X` - Elbow rotation

**Right Arm:**
- `K` / `J` - Shoulder rotation
- `M` / `N` - Elbow rotation

**Left Leg:**
- `D` / `F` - Hip rotation
- `C` / `V` - Knee rotation

**Right Leg:**
- `G` / `H` - Hip rotation
- `,` / `.` - Knee rotation

**General:**
- **`ESC`** - Exit program

## Features

### Implemented
- ✅ **3+ Distinct Scenes** - Day, Night, and Sunset environments with different lighting
- ✅ **3+ Camera Modes** - Orbit, Static Front, and Free camera with full control
- ✅ **5+ Smooth Animations** - Idle walk, one-step, arm wave, head bob, and torso sway (all time-based and smooth)
- ✅ **Phong Lighting** - Ambient, diffuse, and specular shading with material properties
- ✅ **Hierarchical Robot Model** - Articulated body with proper parent-child transformations
- ✅ **Manual Joint Control** - Full control over all robot joints

## Resources

- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [OpenGL Reference](https://www.khronos.org/opengl/wiki/)
- [Learn OpenGL](https://learnopengl.com/)
