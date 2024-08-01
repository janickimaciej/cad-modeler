#version 400 core

layout (location = 0) in vec3 inPosMesh;
layout (location = 1) in vec3 inNormalVectorMesh;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;

out vec3 pos;
out vec3 unnormalizedNormalVector;

void main()
{
	pos = (modelMatrix * vec4(inPosMesh, 1)).xyz;
	unnormalizedNormalVector = (transpose(inverse(modelMatrix)) * vec4(inNormalVectorMesh, 0)).xyz;
	gl_Position = projectionViewMatrix * vec4(pos, 1);
}
