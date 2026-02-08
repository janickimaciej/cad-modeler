#version 420 core

#define controlVerticesCount 4

in vec3 inTessPos[];

uniform mat4 projectionViewMatrix;
uniform ivec2 viewportSize;

layout (vertices = controlVerticesCount) out;
out vec3 tessPos[];
out float divisionY[];

void main()
{
	tessPos[gl_InvocationID] = inTessPos[gl_InvocationID];

	if (gl_InvocationID == 0)
	{
		vec2 tessPosViewport[controlVerticesCount];
		for (int i = 0; i < controlVerticesCount; ++i)
		{
			vec4 clipPos = projectionViewMatrix * vec4(inTessPos[i], 1);
			clipPos /= clipPos.w;
			tessPosViewport[i] = (clipPos.xy + 1) / 2 * viewportSize;
		}
		int polylineLengthViewport = int(length(tessPosViewport[1] - tessPosViewport[0]) +
			length(tessPosViewport[2] - tessPosViewport[1]) +
			length(tessPosViewport[3] - tessPosViewport[2]));
		int approximationLevel = 1;
		int division = max(polylineLengthViewport / approximationLevel, 1);

		divisionY[gl_InvocationID] = min(division / 64 + 1, 64);
		gl_TessLevelOuter[0] = divisionY[0];
		gl_TessLevelOuter[1] = divisionY[0] == 1 ? division : 64;
	}
}
