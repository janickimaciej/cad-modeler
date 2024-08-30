#version 420 core

#define ANAGLYPH_MODE_NONE 0
#define ANAGLYPH_MODE_LEFT_EYE 1
#define ANAGLYPH_MODE_RIGHT_EYE 2

uniform bool isDark;
uniform bool isSelected;
uniform int anaglyphMode;

out vec4 outColor;

vec4 anaglyph(vec4 outColor);

void main()
{
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
