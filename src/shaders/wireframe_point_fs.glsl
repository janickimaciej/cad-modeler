#version 400 core

uniform bool isActive;

out vec4 outColor;

void main()
{
	outColor = isActive ? vec4(1, 1, 0, 1) : vec4(1, 1, 1, 1);
}
