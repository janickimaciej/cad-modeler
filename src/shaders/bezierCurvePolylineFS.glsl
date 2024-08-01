#version 400 core

uniform bool isActive;

out vec4 outColor;

void main()
{
	float brightness = 0.5;
	outColor = isActive ? vec4(brightness, brightness, 0, 1) :
		vec4(brightness, brightness, brightness, 1);
}
