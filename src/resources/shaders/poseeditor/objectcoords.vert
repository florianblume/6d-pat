#version 130
in vec4 vertex;
in vec3 normal;
out vec3 vert;
uniform mat4 projectionMatrix;
void main() {
           vert = vertex.xyz;
           gl_Position = projectionMatrix * vertex;
}
