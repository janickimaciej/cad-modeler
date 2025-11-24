#version 420 core

layout (location = 0) in vec3 inPosQuad;

out vec2 pos;
out vec2 texturePos;

void main()
{
	pos = inPosQuad.xy * 7.5;
	texturePos = (inPosQuad.xy + 1) / 2;
	gl_Position = vec4(inPosQuad, 1);
}
