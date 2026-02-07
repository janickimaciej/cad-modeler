#version 420 core

in vec3 inTessA[];
in vec3 inTessB[];
in vec3 inTessC[];
in vec3 inTessD[];
in vec3 inTessNextPoint[];
in float inTessDt[];

uniform mat4 projectionViewMatrix;
uniform ivec2 viewportSize;

layout (vertices = 1) out;
out vec3 tessA[];
out vec3 tessB[];
out vec3 tessC[];
out vec3 tessD[];
out float tessDt[];
out float divisionY[];

vec2 screenPos(vec3 worldPos);

void main()
{
	tessA[gl_InvocationID] = inTessA[gl_InvocationID];
	tessB[gl_InvocationID] = inTessB[gl_InvocationID];
	tessC[gl_InvocationID] = inTessC[gl_InvocationID];
	tessD[gl_InvocationID] = inTessD[gl_InvocationID];
	tessDt[gl_InvocationID] = inTessDt[gl_InvocationID];

	int polylineLengthScreen =
		int(length(screenPos(inTessNextPoint[gl_InvocationID]) -
			screenPos(inTessA[gl_InvocationID])));
	int approximationLevel = 1;
	int division = max(polylineLengthScreen / approximationLevel, 1);

	divisionY[gl_InvocationID] = min(division / 64 + 1, 64);
	gl_TessLevelOuter[0] = divisionY[0];
	gl_TessLevelOuter[1] = divisionY[0] == 1 ? division : 64;
}

vec2 screenPos(vec3 worldPos)
{
	vec4 clipPos = projectionViewMatrix * vec4(worldPos, 1);
	clipPos /= clipPos.w;
	return (clipPos.xy + 1) / 2 * viewportSize;
}
