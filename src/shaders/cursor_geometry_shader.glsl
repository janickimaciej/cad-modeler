#version 330 core

layout (points) in;

uniform mat4 projectionViewMatrix;

layout (line_strip, max_vertices = 6) out;
out vec3 color;

vec4 transformVec(vec4 axisWorld);
void emitAxis(vec4 axisClip, vec3 axisColor);

void main()
{
	const float size = 1;

	vec4 xAxisClip = transformVec(vec4(size, 0, 0, 0));
	vec3 xAxisColor = vec3(1, 0, 0);
	emitAxis(xAxisClip, xAxisColor);

	vec4 yAxisClip = transformVec(vec4(0, size, 0, 0));
	vec3 yAxisColor = vec3(0, 1, 0);
	emitAxis(yAxisClip, yAxisColor);

	vec4 zAxisClip = transformVec(vec4(0, 0, size, 0));
	vec3 zAxisColor = vec3(0, 0, 1);
	emitAxis(zAxisClip, zAxisColor);
}

vec4 transformVec(vec4 axisWorld)
{
	return projectionViewMatrix * axisWorld;
}

void emitAxis(vec4 axisClip, vec3 axisColor)
{
	color = axisColor;
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + axisClip;
	EmitVertex();
	EndPrimitive();
}
