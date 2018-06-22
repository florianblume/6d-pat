attribute vec4 vertex;
attribute vec3 normal;
varying float x;
varying float y;
varying float z;
varying vec3 vert;
varying vec3 vertNormal;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
void main() {
           x = vertex.x;
           y = vertex.y;
           z = vertex.z;
           vert = vertex.xyz;
           vertNormal = normalMatrix * normal;
           gl_Position = projectionMatrix * vertex;
}
