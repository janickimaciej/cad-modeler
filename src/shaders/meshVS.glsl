#version 420 core

layout (location = 0) in vec3 inPosMesh;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;

void main()
{
	vec4 pos = modelMatrix * vec4(inPosMesh, 1);
	gl_Position = projectionViewMatrix * pos;
}
