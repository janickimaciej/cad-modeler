#version 420 core

in vec3 pos;

out vec4 outColor;

void main()
{
	outColor = vec4(pos.y, 0, 0, 1);
}
