#version 420 core

#define meshDensity 16

layout (quads, equal_spacing, ccw) in;
in vec3 tessPos[];

uniform mat4 projectionViewMatrix;

out vec3 pos;

int index(int ui, int vi);
vec3 deCasteljau2(vec3 a, vec3 b, float t);
vec3 deCasteljau3(vec3 a, vec3 b, vec3 c, float t);
vec3 deCasteljau4(vec3 a, vec3 b, vec3 c, vec3 d, float t);

void main()
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec3 bezierPointsU[4];
	for (int vi = 0; vi < 4; ++vi)
	{
		bezierPointsU[vi] = deCasteljau4(tessPos[index(0, vi)], tessPos[index(1, vi)],
			tessPos[index(2, vi)], tessPos[index(3, vi)], u);
	}

	vec3 bezierPointsV[4];
	for (int ui = 0; ui < 4; ++ui)
	{
		bezierPointsV[ui] = deCasteljau4(tessPos[index(ui, 0)], tessPos[index(ui, 1)],
			tessPos[index(ui, 2)], tessPos[index(ui, 3)], v);
	}

	pos = deCasteljau4(bezierPointsU[0], bezierPointsU[1], bezierPointsU[2], bezierPointsU[3], v);
	gl_Position = projectionViewMatrix * vec4(pos, 1);
}

int index(int ui, int vi)
{
	return 4 * vi + ui;
}

vec3 deCasteljau2(vec3 a, vec3 b, float t)
{
	return mix(a, b, t);
}

vec3 deCasteljau3(vec3 a, vec3 b, vec3 c, float t)
{
	return deCasteljau2(mix(a, b, t), mix(b, c, t), t);
}

vec3 deCasteljau4(vec3 a, vec3 b, vec3 c, vec3 d, float t)
{
	return deCasteljau3(mix(a, b, t), mix(b, c, t), mix(c, d, t), t);
}
