#version 140

uniform mat4 modelViewProjection;

uniform vec4 outlineColor;
uniform sampler2D objectRendering;
uniform vec2 imageSize;

out vec4 fragColor;

void main(void)
{
    int index = 0;
    int size = 2;

    for (int x = -size; x <= size; x++)
    {
        for (int y = -size; y <= size; y++)
        {
            vec2 pos = gl_FragColor.xy + vec2(x, y);
            if (pos.x >= 0 && pos.x <= imageSize.x && pos.y >= 0 && pos.y <= imageSize.y)
            {
                vec4 color = texture2D(objectRendering, pos);
                if (color != vec4(0.0)) {
                    fragColor = outlineColor;
                    return;
                }
            }
        }
    }
    discard;
}
