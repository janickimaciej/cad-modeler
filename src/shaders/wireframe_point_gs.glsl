#version 400 core

layout (points) in;

uniform mat4 projectionViewMatrix;

layout (line_strip, max_vertices = 6) out;

vec4 transformVec(vec4 lineWorld);
void emitLine(vec4 lineClip);

void main()
{
	const float size = 0.1;

	vec4 xLineClip = transformVec(vec4(size, 0, 0, 0));
	emitLine(xLineClip);

	vec4 yLineClip = transformVec(vec4(0, size, 0, 0));
	emitLine(yLineClip);

	vec4 zLineClip = transformVec(vec4(0, 0, size, 0));
	emitLine(zLineClip);
}

vec4 transformVec(vec4 lineWorld)
{
	return projectionViewMatrix * lineWorld;
}

void emitLine(vec4 lineClip)
{
	gl_Position = gl_in[0].gl_Position - lineClip;
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + lineClip;
	EmitVertex();
	EndPrimitive();
}
