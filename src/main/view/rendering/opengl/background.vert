#version 130
in highp vec4 vertex;
in mediump vec4 texCoord;
out mediump vec4 texc;
uniform mediump mat4 matrix;
void main(void)
{
	gl_Position = matrix * vertex;
	texc = texCoord;
}
