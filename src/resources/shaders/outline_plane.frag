#version 150

// Shader to render the actual outlines on a plane

uniform mat4 modelViewProjection;

uniform sampler2D objectModelRenderings;
uniform int samples;
uniform ivec2 imageSize;
uniform ivec4 cameraFrame;
in vec2 texc;

out vec4 fragColor;

void main(void)
{
    int index = 0;
    int size = 1;
    vec4 selectedColor = vec4(1.0, 0.8, 0.3, 1.0);
    vec4 highlightColor = vec4(1.0, 1.0, 1.0, 1.0);

    if (texture2D(objectModelRenderings, gl_FragCoord.xy) != vec4(0.0, 0.0, 0.0, 1.0)) {
        discard;
        return;
    }

    for (int x = -size; x <= size; x++ ) {
        for (int y = -size; y <= size; y++) {
            ivec2 tc = ivec2(gl_FragCoord.xy) + ivec2(x, y);
            if (tc.x >= 0 && tc.x <= imageSize.x && tc.y >= 0 && tc.y <= imageSize.y) {
                vec4 color = texture2D(objectModelRenderings, tc);
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
