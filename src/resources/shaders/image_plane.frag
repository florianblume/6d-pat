#version 150
uniform sampler2DMS texture;
in vec2 texc;
uniform int samples;

void main(void) {
   ivec2 tc = ivec2(floor(textureSize(texture) * texc));
   vec4 color = vec4(0.0);
   for(int i = 0; i < samples; i++) {
       color += texelFetch(texture, tc, i);
   }
   gl_FragColor = color / float(samples);
}
