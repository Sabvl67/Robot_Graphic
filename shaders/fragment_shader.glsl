#version 330 core

/*
  Purpose:
    Shades each fragment using classic Phong lighting components:
    - Ambient:   constant low-level light to avoid fully black surfaces
    - Diffuse:   Lambertian term based on surface normal and light direction
    - Specular:  shiny highlight based on viewer direction and reflection

  Inputs (from vertex shader):
    - FragPos : vec3 — World-space fragment position (for light/view vectors)
    - Normal  : vec3 — World-space surface normal (should be normalized)

  Uniforms (set by the application):
    - lightPos    : vec3 — World-space position of the point/directional light
    - lightColor  : vec3 — RGB intensity/color of the light (e.g., 1.0,1.0,1.0)
    - objectColor : vec3 — Base/albedo color of the material
    - viewPos     : vec3 — World-space camera/eye position

  Shading model:
    1. Ambient
       ambient = ambientStrength * lightColor

    2. Diffuse (Lambert)
       nom      = normalize(Normal)
       lightDir = normalize(lightPos - FragPos)
       diff     = max(dot(nom, lightDir), 0.0)
       diffuse  = diff * lightColor

    3. Specular (Phong)
       viewDir    = normalize(viewPos - FragPos)
       reflectDir = reflect(-lightDir, nom)
       spec       = pow(max(dot(viewDir, reflectDir), 0.0), shininess)
       specular   = specularStrength * spec * lightColor

    Final color:
       result = (ambient + diffuse + specular) * objectColor
       FragColor = vec4(result, 1.0)

  Notes:
    - Normals must be in the SAME space as FragPos (world space here).
      Use inverse-transpose(model) in the vertex shader for correctness.
    - 'shininess' controls highlight size (typical 16–64).
    - Consider applying gamma correction in the framebuffer if needed.
    - For directional light, treat lightDir as a constant direction.
*/


in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

out vec4 FragColor;

void main() {
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff= max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}