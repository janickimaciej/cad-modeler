#version 330 core

layout (location = 0) in vec3 inPositionMesh;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;

void main()
{
	vec4 position = modelMatrix * vec4(inPositionMesh, 1);
	gl_Position = projectionViewMatrix * position;
}
