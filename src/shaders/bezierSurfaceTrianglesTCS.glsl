#version 420 core

#define controlVerticesCount 16
#define meshDensity 64

in vec3 inTessPos[];

layout (vertices = controlVerticesCount) out;
out vec3 tessPos[];

void main()
{
	tessPos[gl_InvocationID] = inTessPos[gl_InvocationID];

	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = meshDensity;
		gl_TessLevelInner[1] = meshDensity;

		gl_TessLevelOuter[0] = meshDensity;
		gl_TessLevelOuter[1] = meshDensity;
		gl_TessLevelOuter[2] = meshDensity;
		gl_TessLevelOuter[3] = meshDensity;
	}
}
