varying highp vec3 vert;
varying highp vec3 vertNormal;
uniform highp vec4 segmentationColor;
void main() {
           // When setting the alpha value of
           // seg color, the seg gets transparent...
           gl_FragColor = segmentationColor;
}
