#version 420 core

#define controlVerticesCount 20

in vec3 inTessPos[];

uniform int lineCount;

layout (vertices = controlVerticesCount) out;
out vec3 tessPos[];

void main()
{
	tessPos[gl_InvocationID] = inTessPos[gl_InvocationID];

	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = lineCount - 1;
		gl_TessLevelInner[1] = lineCount;

		gl_TessLevelOuter[0] = lineCount;
		gl_TessLevelOuter[1] = lineCount - 1;
		gl_TessLevelOuter[2] = lineCount;
		gl_TessLevelOuter[3] = lineCount - 1;
	}
}
