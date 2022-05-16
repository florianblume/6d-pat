#version 130

in vec3 vertexPosition;
uniform mat4 modelViewProjection;

void main(void) {
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
}
