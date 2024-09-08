#version 420 core

#define ANAGLYPH_MODE_NONE 0
#define ANAGLYPH_MODE_LEFT_EYE 1
#define ANAGLYPH_MODE_RIGHT_EYE 2

uniform int anaglyphMode;

out vec4 outColor;

vec4 anaglyph(vec4 outColor);

void main()
{
	outColor = vec4(1, 0, 0, 1);

	outColor = anaglyph(outColor);
}

vec4 anaglyph(vec4 outColor)
{
	switch (anaglyphMode)
	{
		case ANAGLYPH_MODE_LEFT_EYE:
			outColor = vec4(1, 0, 0, 1);
			break;

		case ANAGLYPH_MODE_RIGHT_EYE:
			outColor = vec4(0, 1, 1, 1);
			break;
	}
	return outColor;
}
