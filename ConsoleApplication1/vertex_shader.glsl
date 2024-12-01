#version 330 core
layout(location = 0) in vec3 v0;
layout(location = 1) in vec3 v1;
layout(location = 2) in vec3 v2;

out vec3 fragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculate the normal from three vertices
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;
    fragNormal = normalize(cross(edge1, edge2));
    
    // Transform the vertex position (using v0 as an example)
    gl_Position = projection * view * model * vec4(v0, 1.0);
}
