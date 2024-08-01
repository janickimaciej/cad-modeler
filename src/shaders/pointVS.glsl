#version 400 core

uniform mat4 projectionViewMatrix;
uniform vec3 posWorld;

void main()
{
	gl_Position = projectionViewMatrix * vec4(posWorld, 1);
}
