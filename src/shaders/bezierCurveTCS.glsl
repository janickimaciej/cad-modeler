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
		vec2 tessPosScreen[controlVerticesCount];
		for (int i = 0; i < controlVerticesCount; ++i)
		{
			vec4 clipPos = projectionViewMatrix * vec4(inTessPos[i], 1);
			clipPos /= clipPos.w;
			tessPosScreen[i] = (clipPos.xy + 1) / 2 * viewportSize;
		}
		int polylineLengthScreen = int(length(tessPosScreen[1] - tessPosScreen[0]) +
			length(tessPosScreen[2] - tessPosScreen[1]) +
			length(tessPosScreen[3] - tessPosScreen[2]));
		int approximationLevel = 1;
		int division = max(polylineLengthScreen / approximationLevel, 1);

		divisionY[gl_InvocationID] = min(division / 64 + 1, 64);
		gl_TessLevelOuter[0] = divisionY[0];
		gl_TessLevelOuter[1] = divisionY[0] == 1 ? division : 64;
	}
}
