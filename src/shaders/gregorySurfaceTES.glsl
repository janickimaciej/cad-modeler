#version 420 core

layout (isolines) in;
in vec3 tessPos[];

uniform mat4 projectionViewMatrix;
uniform int lineCount;
uniform bool orientationFlipped;

vec3 deCasteljau2(vec3 a, vec3 b, float t);
vec3 deCasteljau3(vec3 a, vec3 b, vec3 c, float t);
vec3 deCasteljau4(vec3 a, vec3 b, vec3 c, vec3 d, float t);

void main()
{
	float u;
	float v;
	if (orientationFlipped)
	{
		u = gl_TessCoord.y / (1 - 1.0 / lineCount);
		v = gl_TessCoord.x;
	}
	else
	{
		u = gl_TessCoord.x;
		v = gl_TessCoord.y / (1 - 1.0 / lineCount);
	}

	if (u == 0 && v == 0)
	{
		gl_Position = projectionViewMatrix * vec4(tessPos[0], 1);
	}
	else if (u == 1 && v == 0)
	{
		gl_Position = projectionViewMatrix * vec4(tessPos[3], 1);
	}
	else if (u == 0 && v == 1)
	{
		gl_Position = projectionViewMatrix * vec4(tessPos[16], 1);
	}
	else if (u == 1 && v == 1)
	{
		gl_Position = projectionViewMatrix * vec4(tessPos[19], 1);
	}
	else
	{
		vec3 pos[16];
		for (int i = 0; i < 5; ++i)
		{
			pos[i] = tessPos[i];
			pos[11 + i] = tessPos[15 + i];
		}
		pos[5] = (u * tessPos[6] + v * tessPos[5]) / (u + v);
		pos[6] = ((1 - u) * tessPos[7] + v * tessPos[8]) / (1 - u + v);
		pos[7] = tessPos[9];
		pos[8] = tessPos[10];
		pos[9] = (u * tessPos[12] + (1 - v) * tessPos[11]) / (u + 1 - v);
		pos[10] = ((1 - u) * tessPos[13] + (1 - v) * tessPos[14]) / (2 - u - v);

		vec3 bezierV[4];
		for (int i = 0; i < 4; ++i)
		{
			bezierV[i] = deCasteljau4(pos[4 * i], pos[4 * i + 1], pos[4 * i + 2], pos[4 * i + 3],
				u);
		}
		gl_Position = projectionViewMatrix * vec4(deCasteljau4(bezierV[0], bezierV[1], bezierV[2],
			bezierV[3], v), 1);
	}
}

vec3 deCasteljau2(vec3 a, vec3 b, float t)
{
	return mix(a, b, t);
}

vec3 deCasteljau3(vec3 a, vec3 b, vec3 c, float t)
{
	return mix(deCasteljau2(a, b, t), deCasteljau2(b, c, t), t);
}

vec3 deCasteljau4(vec3 a, vec3 b, vec3 c, vec3 d, float t)
{
	return mix(deCasteljau3(a, b, c, t), deCasteljau3(b, c, d, t), t);
}
