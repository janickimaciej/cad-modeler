#version 420 core

layout (location = 0) in vec3 inPosMesh;
layout (location = 1) in vec2 inSurfacePos;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;

out vec2 surfacePos;

void main()
{
	vec4 pos = modelMatrix * vec4(inPosMesh, 1);
	gl_Position = projectionViewMatrix * pos;
	surfacePos = inSurfacePos;
}
