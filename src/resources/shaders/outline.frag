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
    vec4 selectedColor = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 highlightColor = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 currentColor = texelFetch(objectModelRenderings, ivec2(gl_FragCoord.xy), 1);
    if (currentColor.r < 0.1 && currentColor.g < 0.1 && currentColor.b < 0.1) {
        discard;
        return;
    }

    for (int x = -size; x < size; x++ ) {
        for (int y = -size; y < size; y++) {
            ivec2 tc = ivec2(gl_FragCoord.xy) + ivec2(x, y);
            if (tc.x >= 0 && tc.x <= imageSize.x && tc.y >= 0 && tc.y <= imageSize.y) {
                vec4 color = vec4(0.0);
                for(int i = 0; i < 1; i++) {
                    color += texelFetch(objectModelRenderings, tc, i);
                }
                color = color / float(samples);
                if (color.r > 0.1) {
                    fragColor = selectedColor;
                    return;
                } else if (color.g > 0.1) {
                    fragColor = highlightColor;
                    return;
                }
            }
        }
    }
    discard;
}
