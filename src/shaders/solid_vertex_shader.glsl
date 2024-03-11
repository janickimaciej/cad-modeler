#version 330 core

layout (location = 0) in vec3 inPositionMesh;
layout (location = 1) in vec3 inNormalVectorMesh;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;

out vec4 position;
out vec4 normalVector;

void main()
{
	position = modelMatrix * vec4(inPositionMesh, 1);
	normalVector = normalize(modelMatrix * vec4(inNormalVectorMesh, 0));
	gl_Position = projectionViewMatrix * position;
}
