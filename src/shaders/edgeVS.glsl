#version 420 core

layout (location = 0) in vec3 inPosQuad;

out vec2 texturePos;

void main()
{
	texturePos = (inPosQuad.xy + 1) / 2;
	gl_Position = vec4(inPosQuad, 1);
}
