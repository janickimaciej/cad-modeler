#version 420 core

in vec2 texturePos;

uniform sampler2D textureSampler;
uniform float level;

out vec4 outColor;

void main()
{
	const float eps = 1e-9f;
	float height = texture(textureSampler, texturePos).r;
	outColor = vec4(height - level >= eps, 0, 0, 1);
}
