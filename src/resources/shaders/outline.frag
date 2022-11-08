#version 150

uniform mat4 modelViewProjection;

uniform sampler2DMS objectModelRenderings;
uniform int samples;
uniform ivec2 imageSize;
in vec2 texc;

out vec4 fragColor;

void main(void)
{
    int index = 0;
    int size = 2;
    vec4 highlightColor = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 selectedColor = vec4(0.0, 1.0, 0.0, 1.0);

    for (int x = -size; x <= size; x++) {
        for (int y = -size; y <= size; y++) {
            ivec2 pos = ivec2(gl_FragCoord.xy) + ivec2(x, y);
            if (pos.x >= 0 && pos.y <= imageSize.x && pos.y >= 0 && pos.y <= imageSize.y) {
                ivec2 tc = pos;
                vec4 color = vec4(0.0);
                for(int i = 0; i < samples; i++) {
                    color += texelFetch(objectModelRenderings, tc, i);
                }
                if (color.a != 0.0) {
                    fragColor = selectedColor;
                } else if (color.b != 0.0) {
                    fragColor = highlightColor;
                }
            }
        }
    }
    discard;
}
