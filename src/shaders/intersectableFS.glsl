#version 420 core

#define ANAGLYPH_MODE_NONE 0
#define ANAGLYPH_MODE_LEFT_EYE 1
#define ANAGLYPH_MODE_RIGHT_EYE 2

#define TRIM_NONE 0
#define TRIM_RED 1
#define TRIM_GREEN 2

in vec2 surfacePos;

uniform bool isDark;
uniform bool isSelected;
uniform int anaglyphMode;
uniform bool useTrim;
uniform int trimSide;
uniform sampler2D trimTextureSampler;
uniform vec2 textureMin;
uniform vec2 textureMax;

out vec4 outColor;

vec4 anaglyph(vec4 outColor);

void main()
{
	if (useTrim)
	{
		vec2 texturePos = textureMin + (textureMax - textureMin) * surfacePos;
		vec3 textureColor = texture(trimTextureSampler, texturePos).xyz;
		if (trimSide == TRIM_RED && textureColor.r >= 0.25 ||
			trimSide == TRIM_GREEN && textureColor.r < 0.25)
		{
			discard;
		}
	}

	float brightness = isDark ? 0.5 : 1;
	outColor = isSelected ? vec4(brightness, brightness, 0, 1) :
		vec4(brightness, brightness, brightness, 1);

	outColor = anaglyph(outColor);
}

vec4 anaglyph(vec4 outColor)
{
	switch (anaglyphMode)
	{
		case ANAGLYPH_MODE_LEFT_EYE:
			outColor = vec4(outColor.r, 0, 0, outColor.a);
			break;

		case ANAGLYPH_MODE_RIGHT_EYE:
			outColor = vec4(0, outColor.g, outColor.b, outColor.a);
			break;
	}
	return outColor;
}
