#version 130
in highp vec3 vert;
void main() {
           gl_FragData[0] = vec4(vert, 1.0);
}
