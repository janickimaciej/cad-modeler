#version 420 core

uniform bool isSelected;

out vec4 outColor;

void main()
{
	float brightness = 0.5;
	outColor = isSelected ? vec4(brightness, brightness, 0, 1) :
		vec4(brightness, brightness, brightness, 1);
}
