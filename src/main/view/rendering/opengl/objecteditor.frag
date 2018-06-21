varying highp vec3 vert;
varying highp vec3 vertNormal;
uniform highp vec3 lightPos;
void main() {
           highp vec3 L = normalize(lightPos - vert);
           highp float NL = max(dot(normalize(vertNormal), L), 0.0);
           highp vec3 color = vec3(0.7, 0.7, 0.7);
           highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);
           gl_FragData[0] = vec4(1.0, 1.0, 1.0, 1.0);
           gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);
}
