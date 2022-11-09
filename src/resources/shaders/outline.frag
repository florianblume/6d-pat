#version 140

uniform vec4 color;
out vec4 fragColor0;
out vec4 fragColor1;

void main(void) {
    fragColor0 = color;
    fragColor1 = color;
}
