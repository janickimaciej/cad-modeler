#version 420 core

layout (location = 0) in vec3 inPosQuad;

uniform ivec2 heightmapSize;
uniform ivec2 viewportSize;
uniform ivec2 viewportOffset;

out vec2 pos;
out vec2 texturePos;

void main()
{
	texturePos = ((inPosQuad.xy + 1) / 2 * viewportSize + viewportOffset) / heightmapSize;
	pos = (texturePos * 2 - 1) * 7.5;
	gl_Position = vec4(inPosQuad, 1);
}
