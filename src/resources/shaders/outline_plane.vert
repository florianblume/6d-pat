#version 140

// Shader to render the actual outlines on a plane

in vec3 vertexPosition;
in vec2 vertexTexCoord;
out vec2 texc;

uniform mat4 modelViewProjection;

void main()
{
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
    texc = vertexTexCoord;
}
