#version 130

uniform vec3 color;
out vec4 fragColor;

void main(void) {
    fragColor = vec4(color, 1.0);
}
