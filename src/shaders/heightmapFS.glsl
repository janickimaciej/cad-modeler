#version 420 core

in vec2 pos;
in vec2 texturePos;

uniform float radius;
uniform bool flatCutter;
uniform float base;
uniform float pathLevel;
uniform sampler2D textureSampler;

out vec4 outColor;

void main()
{
	/*float textureRadius = radius / 15;
	const int samplingResolution = 64;
	float dx = 2 * textureRadius / samplingResolution;
	float dy = dx;
	float maxHeight = flatCutter ? 0 : radius;
	for (int i = 0; i <= samplingResolution; ++i)
	{
		float x = texturePos.x - textureRadius + i * dx;
		for (int j = 0; j <= samplingResolution; ++j)
		{
			float y = texturePos.y - textureRadius + j * dy;
			float distanceSquared = pow(x - texturePos.x, 2) + pow(y - texturePos.y, 2);
			if (pow(textureRadius, 2) - distanceSquared >= 0)
			{
				float cutterOffset = flatCutter ? 0 :
					15 * sqrt(pow(textureRadius, 2) - distanceSquared);
				float height = max(texture(textureSampler, vec2(x, y)).r, pathLevel);
				maxHeight = max(maxHeight, height + cutterOffset);
			}
		}
	}
	outColor = vec4(base + maxHeight, 0, 0, 1);*/

	float maxHeight = texture(textureSampler, texturePos).r + (flatCutter ? 0 : radius);
	float textureRadius = radius / 15;
	const int radiusResolution = 32;
	const int angleResolution = 128;
	float dr = textureRadius / radiusResolution;
	float dAlpha = 2 * 3.1415927 / angleResolution;
	for (int i = 1; i <= radiusResolution; ++i)
	{
		float r = i * dr;
		for (int j = 0; j < angleResolution; ++j)
		{
			float alpha = j * dAlpha;
			float x = texturePos.x + r * cos(alpha);
			float y = texturePos.y + r * sin(alpha);
			float cutterOffset = flatCutter ? 0 :
				15 * sqrt(pow(textureRadius, 2) - pow(r, 2));
			float height = max(texture(textureSampler, vec2(x, y)).r, pathLevel);
			maxHeight = max(maxHeight, height + cutterOffset);
		}
	}
	outColor = vec4(base + maxHeight, 0, 0, 1);
}
