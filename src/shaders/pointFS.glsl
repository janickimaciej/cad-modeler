#version 400 core

uniform bool isVirtual;
uniform bool isActive;

out vec4 outColor;

void main()
{
	float virtualBrightness = 0.5;
	if (isActive)
	{
		outColor = vec4(1, 1, 0, 1);
	}
	else if (isVirtual)
	{
		outColor = vec4(virtualBrightness, virtualBrightness, virtualBrightness, 1);
	}
	else
	{
		outColor = vec4(1, 1, 1, 1);
	}
}
