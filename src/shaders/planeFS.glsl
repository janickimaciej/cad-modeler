#version 420 core

#define ORTHOGRAPHIC_CAMERA 0
#define PERSPECTIVE_CAMERA 1
#define EPS 1e-6

#define ANAGLYPH_MODE_NONE 0
#define ANAGLYPH_MODE_LEFT_EYE 1
#define ANAGLYPH_MODE_RIGHT_EYE 2

in vec3 nearPlanePoint;
in vec3 farPlanePoint;

uniform mat4 projectionViewMatrix;
uniform float scale;
uniform int cameraType;
uniform int anaglyphMode;

out vec4 outColor;

float depth(vec3 posWorld);
vec4 gridColor(vec3 posWorld);
vec4 anaglyph(vec4 outColor);

void main()
{
	float t = -nearPlanePoint.y / (farPlanePoint.y - nearPlanePoint.y);
	vec3 posWorld = nearPlanePoint + t * (farPlanePoint - nearPlanePoint);
	gl_FragDepth =
		min((gl_DepthRange.diff * depth(posWorld) + gl_DepthRange.near + gl_DepthRange.far) / 2,
			gl_DepthRange.far - EPS);
	outColor = gridColor(posWorld);
	
	if (cameraType == PERSPECTIVE_CAMERA)
	{
		outColor *= float(t > 0);
	}

	outColor = anaglyph(outColor);
}

float depth(vec3 posWorld)
{
	vec4 posClip = projectionViewMatrix * vec4(posWorld, 1);
	return posClip.z / posClip.w;
}

vec4 gridColor(vec3 posWorld)
{
	vec2 posGrid = posWorld.xz / scale;
	vec2 posGridDeriv = fwidth(posGrid);
	vec2 distFromGrid = abs(fract(posGrid - 0.5) - 0.5) / posGridDeriv;
	float minDistFromGrid = min(distFromGrid.x, distFromGrid.y);
	float brightness = 0.2;
	vec4 color = vec4(brightness, brightness, brightness, 1.0 - min(minDistFromGrid, 1.0));
	const float axisWidth = 5;

	float posGridDerivZ = min(posGridDeriv.y, 1);
	if (posWorld.z > -axisWidth * posGridDerivZ &&
		posWorld.z < axisWidth * posGridDerivZ)
	{
		color.r = 1.0;
	}

	float posGridDerivX = min(posGridDeriv.x, 1);
	if (posWorld.x > -axisWidth * min(posGridDerivX, 1) &&
		posWorld.x < axisWidth * min(posGridDerivX, 1))
	{
		color.b = 1.0;
	}
	return color;
}

vec4 anaglyph(vec4 outColor)
{
	float brightness = 0.2;
	if (outColor.r == 1 || outColor.b == 1)
	{
		brightness = 1;
	}
	switch (anaglyphMode)
	{
		case ANAGLYPH_MODE_LEFT_EYE:
			outColor = vec4(brightness, 0, 0, outColor.a);
			break;

		case ANAGLYPH_MODE_RIGHT_EYE:
			outColor = vec4(0, brightness, brightness, outColor.a);
			break;
	}
	return outColor;
}
