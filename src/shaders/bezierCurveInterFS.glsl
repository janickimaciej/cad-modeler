#version 420 core

uniform bool isSelected;

out vec4 outColor;

void main()
{
	outColor = isSelected ? vec4(1, 1, 0, 1) : vec4(1, 1, 1, 1);
}
