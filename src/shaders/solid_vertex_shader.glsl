#version 330 core

layout (location = 0) in vec3 inPositionMesh;
layout (location = 1) in vec3 inNormalVectorMesh;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;

out vec3 position;
out vec3 unnormalizedNormalVector;

void main()
{
	position = (modelMatrix * vec4(inPositionMesh, 1)).xyz;
	unnormalizedNormalVector = (modelMatrix * vec4(inNormalVectorMesh, 0)).xyz;
	gl_Position = projectionViewMatrix * vec4(position, 1);
}
