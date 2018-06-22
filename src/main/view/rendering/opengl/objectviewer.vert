#version 130
in vec4 vertex;
in vec3 normal;
out vec3 vert;
out vec3 vertNormal;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
void main() {
           vert = vertex.xyz;
           vertNormal = normalMatrix * normal;
           gl_Position = projectionMatrix * vertex;
}
