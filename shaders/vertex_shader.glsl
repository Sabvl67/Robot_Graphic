#version 330 core

/*
  This vertex shader transforms each vertex from object space into 
  clip space using model-view-projection pipeline, while prepare per-vertex data for lighting calculations in the 
  fragment shader.

  The shader takes two vertex attributes:
    - aPos:    The vertex position in object (model) coordinates.
    - aNormal: The vertex normal vector in object coordinates.

  Three uniform matrices are provided by the C++ application:
    - model:       Positions and orients the object in world space.
    - view:        Represents the camera’s viewpoint (world → camera space).
    - projection:  Projects 3D camera-space coordinates into 2D clip space.

  The shader performs the following transformations:
    1. Converts the vertex position (aPos) into world-space coordinates 
       by multiplying it with the model matrix. The resulting world-space 
       position is stored in FragPos for use in lighting computations.
    2. Transforms the vertex normal (aNormal) using the inverse-transpose 
       of the model matrix to preserve its correct direction even under 
       non-uniform scaling. This adjusted normal is passed to the fragment 
       shader as Normal.
    3. Computes the final clip-space position (gl_Position) by applying 
       the projection and view matrices to the transformed vertex position.
*/


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}