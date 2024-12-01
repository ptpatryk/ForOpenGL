#version 330 core
in vec3 fragNormal;
out vec4 color;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // Normalize the normal and light direction
    vec3 norm = normalize(fragNormal);
    vec3 light = normalize(lightDir);
    
    // Calculate the diffuse component
    float diff = max(dot(norm, light), 0.0);
    
    // Calculate the final color
    vec3 result = diff * lightColor * objectColor;
    color = vec4(result, 1.0);
    //color = vec4(1f,1.0f, 1.0f, 1.0f);
}
