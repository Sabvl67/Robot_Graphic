#version 330 core

// Fragment shader: Phong lighting (ambient + diffuse + specular)

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform vec3 objectCol;
uniform vec3 camPos;

out vec4 FragColor;

void main() {
    // Ambient
    vec3 ambient = 0.3 * lightCol;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightCol;

    // Specular
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * lightCol;

    vec3 result = (ambient + diffuse + specular) * objectCol;
    FragColor = vec4(result, 1.0);
}