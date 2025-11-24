#version 420 core

in vec2 texturePos;

uniform sampler2D textureSampler;

out vec4 outColor;

void main()
{
	float height = texture(textureSampler, texturePos).r;
	outColor = vec4(height, height - 1, height - 2, 1);
}
