#version 150

// Shader to render the actual outlines on a plane

uniform mat4 modelViewProjection;

uniform sampler2D outlineRendering;
uniform ivec2 imageSize;
uniform ivec4 cameraFrame;
in vec2 texc;

out vec4 fragColor;

void main(void)
{
    int index = 0;
    float size = 0.001;
    vec4 selectedColor = vec4(1.0, 0.8, 0.3, 1.0);
    vec4 highlightColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec2 newTexc = vec2(texc.x, 1.0 - texc.y);

    if (texture2D(outlineRendering, newTexc) != vec4(0.0, 0.0, 0.0, 1.0)) {
        discard;
        return;
    }

    for (float x = -size; x <= size; x+=0.001 ) {
        for (float y = -size; y <= size; y+=0.001) {
            vec2 tc = texc + vec2(x, y);
            tc.y = 1.0 - tc.y;
            if (tc.x >= 0.0 && tc.x <= 1.0 && tc.y >= 0.0 && tc.y <= 1.0) {
                vec4 color = texture2D(outlineRendering, tc);
                if (color.g > 0.1) {
                    fragColor = highlightColor;
                    return;
                }
            }
        }
    }
    discard;
}
