#version 420 core

in vec2 pos;
in vec2 texturePos;

uniform float radius;
uniform bool flatCutter;
uniform float base;
uniform float pathLevel;
uniform sampler2D textureSampler;

out vec4 outColor;

vec2 pos2TexturePos(vec2 pos)
{
	return (pos + 7.5) / 15.0;
}

void main()
{
	float maxHeight = max(texture(textureSampler, texturePos).r, pathLevel)
		+ (flatCutter ? 0 : radius);
	const int radiusResolution = 32;
	const int angleResolution = 128;
	float dr = radius / radiusResolution;
	float dAlpha = 2 * 3.1415927 / angleResolution;
	for (int i = 1; i <= radiusResolution; ++i)
	{
		float r = i * dr;
		for (int j = 0; j < angleResolution; ++j)
		{
			float alpha = j * dAlpha;
			float x = pos.x + r * cos(alpha);
			float y = pos.y + r * sin(alpha);
			float cutterOffset = flatCutter ? 0 : sqrt(pow(radius, 2) - pow(r, 2));
			float height = max(texture(textureSampler, pos2TexturePos(vec2(x, y))).r, pathLevel);
			maxHeight = max(maxHeight, height + cutterOffset);
		}
	}
	outColor = vec4(base + maxHeight, 0, 0, 1);
}
