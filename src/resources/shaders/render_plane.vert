#version 150

in vec3 vertex;
in vec2 texCoord;

uniform mat4 matrix;

out vec2 texc;

void main(void) {
    gl_Position = matrix * vec4(vertex.xyz, 1.0);
    texc = texCoord;
}
