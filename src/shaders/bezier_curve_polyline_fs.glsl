#version 400 core

uniform bool isActive;

out vec4 outColor;

void main()
{
	float brightness = 0.5;
	outColor = isActive ? vec4(brightness, brightness, 0, brightness) :
		vec4(brightness, brightness, brightness, brightness);
}
