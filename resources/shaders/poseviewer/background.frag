#version 130
uniform sampler2D texture;
in mediump vec4 texc;
void main(void)
{
	gl_FragColor = texture2D(texture, texc.st);
}
