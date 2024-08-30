#version 420 core

#define ANAGLYPH_MODE_NONE 0
#define ANAGLYPH_MODE_LEFT_EYE 1
#define ANAGLYPH_MODE_RIGHT_EYE 2

in vec3 color;

uniform int anaglyphMode;

out vec4 outColor;

vec4 anaglyph(vec4 outColor);

void main()
{
	outColor = vec4(color, 1);

	outColor = anaglyph(outColor);
}

vec4 anaglyph(vec4 outColor)
{
	switch (anaglyphMode)
	{
		case ANAGLYPH_MODE_LEFT_EYE:
			outColor = vec4(1, 0, 0, outColor.a);
			break;

		case ANAGLYPH_MODE_RIGHT_EYE:
			outColor = vec4(0, 1, 1, outColor.a);
			break;
	}
	return outColor;
}
