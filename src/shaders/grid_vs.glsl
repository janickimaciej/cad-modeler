#version 400 core

layout (location = 0) in vec3 inPosCanvas;

uniform mat4 projectionViewMatrixInverse;

out vec3 nearPlanePoint;
out vec3 farPlanePoint;

vec3 clipToWorld(vec3 posClip);

void main()
{
	nearPlanePoint = clipToWorld(vec3(inPosCanvas.xy, -1));
	farPlanePoint = clipToWorld(vec3(inPosCanvas.xy, 1));
	gl_Position = vec4(inPosCanvas, 1);
}

vec3 clipToWorld(vec3 posClip)
{
	vec4 posWorld = projectionViewMatrixInverse * vec4(posClip, 1);
	return posWorld.xyz / posWorld.w;
}
