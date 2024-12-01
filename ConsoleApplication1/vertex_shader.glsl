	#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
out vec3 fragNormal;
out vec3 fragPosition;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec3 p1 = position;
    vec3 p2 = vec3(gl_VertexID == 0 ? 1.0 : 0.0, gl_VertexID == 1 ? 1.0 : 0.0, gl_VertexID == 2 ? 1.0 : 0.0);
    vec3 p3 = vec3(gl_VertexID == 0 ? 0.0 : 1.0, gl_VertexID == 1 ? 0.0 : 1.0, gl_VertexID == 2 ? 0.0 : 1.0);

    vec3 U = p2 - p1;
    vec3 V = p3 - p1;
    vec3 calculatedNormal = normalize(cross(U, V));

    fragPosition = vec3(model * vec4(position, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * calculatedNormal;
    gl_Position = projection * view * vec4(fragPosition, 1.0);
}