# OpenGL Hierarchical Robot - Learning Notes

## Table of Contents
1. [Matrix Transformations](#matrix-transformations)
2. [Shader Basics](#shader-basics)
3. [Lighting Concepts](#lighting-concepts)
4. [Vertex Buffer Objects (VBO/VAO)](#vbo-vao)
5. [Shader Manager](#shader-manager)

---

## Matrix Transformations

### Question 1: Matrix Multiplication Order

**Problem:** If you want to:
1. First rotate an arm 45° around its shoulder
2. Then move it 2 units to the left

Which order is correct?
- **Option A:** `translate(2, 0, 0) × rotate(45°)` ✅ CORRECT
- **Option B:** `rotate(45°) × translate(2, 0, 0)` ❌ WRONG

**Answer: Option A**

**Explanation:**
Matrix multiplication is read **RIGHT to LEFT**. The rightmost transformation happens first.

```cpp
// CORRECT: Rotate first, then translate
glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(2, 0, 0))  // Second (move to shoulder)
                    * glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0,0,1)); // First (rotate at origin)
```

**Why this order?**
- You want to rotate the arm around its local origin (shoulder joint)
- Then position it at the shoulder location
- If you translate first, then rotate, the arm rotates around the wrong point!

**Visual:**
```
Wrong (translate × rotate):     Correct (rotate × translate):
    Arm rotates around           Arm rotates around
    a distant point!             its shoulder joint
         |                              |
         |___/  ← weird              ___/  ← natural
                                     |
```

---

### Question 2: Hierarchy Understanding

**Problem:**
```cpp
glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Move up 1
glm::mat4 torso = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f)); // Move up 2 more
```

What is the final Y position of the torso in world space?

**Answer: 3.0**

**Explanation:**
- Base is at Y = 1.0
- Torso is 2.0 units above the base
- Total: 1.0 + 2.0 = 3.0

**Key Concept:** Children inherit parent transformations
```cpp
torso = base × local_torso_offset
      = "move up 1" × "move up 2"
      = "move up 3" (combined)
```

**Hierarchy Example:**
```
World Origin (0, 0, 0)
    ↓
Base at (0, 1, 0)              ← base transform
    ↓
Torso at (0, 3, 0)             ← base × torso_offset = (0, 1+2, 0)
    ↓
Arm at (2, 3, 0)               ← base × torso × arm_offset
```

---

## Lighting Concepts

### Question 3: Diffuse Lighting (Dot Product)

**Problem:**
In the fragment shader:
```glsl
float diff = max(dot(norm, lightDir), 0.0);
```

What does `dot(norm, lightDir)` tell us?

**Answer: Option B - How directly the light hits the surface (angle)**

**Explanation:**

**Dot Product of Two Unit Vectors = Cosine of Angle**

```
norm = surface normal (perpendicular to surface)
lightDir = direction from surface to light

dot(norm, lightDir) = cos(angle between them)
```

**Scenarios:**
```
1. Light directly hits surface:
   norm ↑  lightDir ↑
   angle = 0°
   dot = cos(0°) = 1.0  ← Maximum brightness

2. Light at 60° angle:
   norm ↑  lightDir ↗
   angle = 60°
   dot = cos(60°) = 0.5  ← Half brightness

3. Light parallel to surface:
   norm ↑  lightDir →
   angle = 90°
   dot = cos(90°) = 0.0  ← No light

4. Light from behind:
   norm ↑  lightDir ↓
   angle = 180°
   dot = cos(180°) = -1.0  ← Clamped to 0 by max()
```

**Why `max(..., 0.0)`?**
- Negative values mean light is behind the surface
- We clamp to 0 (no negative light!)

---

## Shader Basics

### What Are Shaders?

**Shaders = Small programs that run on the GPU**

In Modern OpenGL Core Profile, you **must** write shaders. The CPU can't draw directly.

**Two Required Shaders:**
1. **Vertex Shader** - Runs once per vertex, transforms 3D positions
2. **Fragment Shader** - Runs once per pixel, determines colors

**Pipeline:**
```
C++ Code → Vertex Shader → Rasterizer → Fragment Shader → Screen
           (per vertex)    (creates     (per pixel)
                           pixels)
```

---

### Vertex Shader Explained

**Purpose:** Transform vertices from object space → screen space

```glsl
#version 330 core

// INPUT: From your C++ VBO
layout (location = 0) in vec3 aPos;     // Vertex position
layout (location = 1) in vec3 aNormal;  // Surface normal

// UNIFORMS: Same for all vertices (from C++)
uniform mat4 model;       // Object transform
uniform mat4 view;        // Camera transform
uniform mat4 projection;  // Perspective projection

// OUTPUT: To fragment shader
out vec3 FragPos;  // World-space position
out vec3 Normal;   // World-space normal

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
```

**Key Line:**
```glsl
gl_Position = projection * view * model * position;
                  ↑         ↑      ↑        ↑
              3D→2D    Camera   Object   Vertex
```

**Read RIGHT to LEFT:**
1. Start with vertex in object space
2. `model` → Move to world space
3. `view` → Move to camera space
4. `projection` → Project to 2D screen space

---

### Fragment Shader Explained

**Purpose:** Calculate color for each pixel using lighting

```glsl
#version 330 core

// INPUT: From vertex shader (interpolated)
in vec3 FragPos;
in vec3 Normal;

// UNIFORMS: Lighting parameters
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

// OUTPUT: Final pixel color
out vec4 FragColor;

void main() {
    // 1. Ambient (base light)
    vec3 ambient = 0.3 * lightColor;

    // 2. Diffuse (angle-based brightness)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 3. Specular (shiny highlights)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * lightColor;

    // Combine all
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
```

**Phong Lighting Model:**
```
Final Color = (Ambient + Diffuse + Specular) × Object Color

Ambient  = Constant base light (no darkness)
Diffuse  = Angle-dependent brightness
Specular = Shiny highlights (reflection)
```

---

### Why vec4 Instead of vec3?

**Question:** Why do we write `vec4(aPos, 1.0)` instead of `vec3(aPos)`?

**Answer:** Homogeneous coordinates (4D math for 3D graphics)

**Explanation:**
```
vec4(x, y, z, w)

w = 1.0 → Position (affected by translation)
w = 0.0 → Direction (NOT affected by translation)

Example:
vec4(2, 3, 4, 1.0) → Point in space
vec4(0, 1, 0, 0.0) → "Up" direction (for normals)
```

**Why we need this:**
- 3×3 matrices can't represent translations
- 4×4 matrices can do rotation + translation + scaling
- Standard in computer graphics (projective geometry)

---

## Shader Manager

### Question 1: Why glDeleteShader After Linking?

**Answer:** Once shaders are linked into a program, the individual shader objects are no longer needed.

**Analogy:**
```
Shaders = Source code (.cpp files)
Program = Compiled executable

After compiling and linking C++:
- You have an executable
- You don't need the .o object files anymore
```

**Code:**
```cpp
glAttachShader(program, vertexShader);
glAttachShader(program, fragmentShader);
glLinkProgram(program);

// Shaders are now "baked" into the program
glDeleteShader(vertexShader);    // Free memory
glDeleteShader(fragmentShader);  // Free memory
```

---

### Question 2: What Does glShaderSource Do?

**Answer:** Sends GLSL source code (as a string) to OpenGL.

**Code:**
```cpp
const char* shaderSource = "#version 330 core\nvoid main() { ... }";
glShaderSource(shader, 1, &shaderSource, NULL);
           //  ↑      ↑        ↑          ↑
           //  |      |        |          |
           // shader  num    array of    lengths
           // object  strings  strings   (NULL = auto)
```

**Parameters:**
- `shader` - The shader object to attach source to
- `1` - Number of strings (we have 1 shader file)
- `&shaderSource` - Pointer to array of strings
- `NULL` - Auto-calculate string lengths

---

### Question 3: Why Check GL_COMPILE_STATUS?

**Answer:** To catch GLSL syntax errors and print helpful error messages.

**Example Error:**
```glsl
// WRONG: Missing semicolon
void main() {
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0)  // ← ERROR!
}
```

**Error Checking Code:**
```cpp
glCompileShader(shader);

GLint success;
glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "SHADER ERROR:\n" << infoLog << std::endl;
}
```

**Output:**
```
ERROR: 0:3: ';' : syntax error
```

---

## VBO/VAO

### What Are VBO and VAO?

**VBO (Vertex Buffer Object):**
- Block of GPU memory storing vertex data
- Contains positions, normals, colors, etc.

**VAO (Vertex Array Object):**
- Stores the *format* of VBO data
- Tells OpenGL how to interpret the VBO

**Analogy:**
```
VBO = Raw data file (bytes)
VAO = File format specification (CSV, JSON, etc.)
```

**Example:**
```cpp
float vertices[] = {
    // Positions       // Normals
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // Vertex 1
     0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // Vertex 2
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Vertex 3
};

// Create VBO - upload data to GPU
GLuint VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// Create VAO - describe data format
GLuint VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

// Position attribute (location 0)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// Normal attribute (location 1)
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
glEnableVertexAttribArray(1);
```

**glVertexAttribPointer Parameters:**
```
glVertexAttribPointer(location, size, type, normalized, stride, offset)
                          ↓       ↓     ↓        ↓         ↓       ↓
                          0       3   FLOAT   FALSE    6*4bytes  0

location - Matches shader layout (location = 0)
size     - Number of components (3 for vec3)
type     - Data type (float)
normalized - Normalize to [0,1]? (false for positions)
stride   - Bytes between consecutive vertices (6 floats = 24 bytes)
offset   - Bytes to first component (0 for position, 12 for normal)
```

---

## Key Concepts Summary

### Transformation Hierarchy
```cpp
// Pattern for hierarchy
glm::mat4 parent = ...;
glm::mat4 child = parent * local_transform;

// Example
glm::mat4 base = glm::translate(I, vec3(0, 1, 0));
glm::mat4 torso = base * glm::translate(I, vec3(0, 2, 0));
glm::mat4 arm = torso * glm::translate(I, vec3(1, 0, 0))
                      * glm::rotate(I, angle, vec3(0,0,1));
```

### Matrix Order
```
Read right to left:
translate * rotate * scale

1. Scale (change size)
2. Rotate (orient)
3. Translate (position)
```

### Shader Workflow
```
1. C++ loads .glsl files
2. OpenGL compiles on GPU
3. C++ links vertex + fragment
4. C++ uses program to draw
```

### Lighting Formula
```glsl
ambient = 0.3 * lightColor
diffuse = max(dot(normal, lightDir), 0.0) * lightColor
specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess) * lightColor

final = (ambient + diffuse + specular) * objectColor
```

---

## Common Mistakes to Avoid

1. **Forgetting `#version 330 core`** - Must be first line of shader
2. **Wrong matrix order** - Remember: read right to left
3. **Not checking shader compilation** - Always check GL_COMPILE_STATUS
4. **Using vec3 instead of vec4** - Positions need w=1.0 for homogeneous coords
5. **Wrong VBO stride/offset** - Must match your vertex data layout
6. **Not normalizing normals** - Always `normalize(Normal)` in fragment shader
7. **Negative dot product** - Use `max(dot(...), 0.0)` to clamp

---

## Quick Reference

### GLM Functions
```cpp
glm::mat4 I = glm::mat4(1.0f);                    // Identity matrix
glm::translate(I, glm::vec3(x, y, z))             // Translation
glm::rotate(I, angle, glm::vec3(x, y, z))         // Rotation around axis
glm::scale(I, glm::vec3(sx, sy, sz))              // Scale
glm::lookAt(eye, center, up)                      // View matrix
glm::perspective(fov, aspect, near, far)          // Projection matrix
```

### OpenGL Shader Functions
```cpp
glCreateShader(type)                // Create shader object
glShaderSource(shader, count, src, len)  // Attach source
glCompileShader(shader)             // Compile
glGetShaderiv(shader, param, ptr)   // Get shader info
glGetShaderInfoLog(shader, size, len, log)  // Get errors

glCreateProgram()                   // Create program
glAttachShader(program, shader)     // Attach shader
glLinkProgram(program)              // Link
glUseProgram(program)               // Activate

glUniformMatrix4fv(loc, count, transpose, ptr)  // Send matrix
glUniform3fv(loc, count, ptr)       // Send vec3
```

---

## Next Steps

- [ ] Implement shader manager (shader.cpp)
- [ ] Create cube geometry with VBO/VAO
- [ ] Set up camera (glm::lookAt)
- [ ] Add lighting uniforms
- [ ] Build robot hierarchy
- [ ] Animate one motion
- [ ] Test in lab environment

---

*Last updated: 2025-11-03*
*Review this file before exams or when debugging!*
