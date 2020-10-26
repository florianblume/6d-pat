#version 150 core

uniform mat4 modelViewProjection;

uniform vec2 clicks[10];
uniform vec3 colors[10];
uniform int clickCount;
uniform float circumfence;

out vec4 fragColor;

void main(void)
{
    bool isClicked = false;
    int index = 0;

    for (int i = 0; i < clickCount; i++)
    {
       vec2 delta = abs(gl_FragCoord.xy - clicks[i]);
       if (pow(delta.r, 2) + pow(delta.g, 2) <= pow(circumfence, 2))
       {
          fragColor = vec4(colors[i], 1);
          return;
       }
    }
    discard;
}
