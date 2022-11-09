#version 150
uniform sampler2DMS texture;
in vec2 texc;
uniform int samples;

out vec4 fragColor;

void main(void) {
   ivec2 tc = ivec2(floor(textureSize(texture) * texc));
   vec4 color = vec4(0.0);
   for(int i = 0; i < samples; i++) {
       color += texelFetch(texture, tc, i);
   }
   fragColor = color / float(samples);
}

/*
#version 150
uniform sampler2D texture;
in vec2 texc;
uniform int samples;

out vec4 fragColor;

void main(void) {
   vec4 color = texture2D(texture, texc);
   fragColor = color;
}

  */
