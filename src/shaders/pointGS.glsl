#version 420 core

layout (points) in;

uniform mat4 projectionViewMatrix;

layout (triangle_strip, max_vertices = 36) out;

vec4 transformVec(vec4 lineWorld);
void emitTriangle(vec4 vertexClip0, vec4 vertexClip1, vec4 vertexClip2);

void main()
{
	const float size = 0.1;

	vec4 points[8] =
	{
		transformVec(vec4(-size, -size, -size, 0)),
		transformVec(vec4(-size, -size, size, 0)),
		transformVec(vec4(-size, size, -size, 0)),
		transformVec(vec4(-size, size, size, 0)),
		transformVec(vec4(size, -size, -size, 0)),
		transformVec(vec4(size, -size, size, 0)),
		transformVec(vec4(size, size, -size, 0)),
		transformVec(vec4(size, size, size, 0))
	};

	emitTriangle(points[0], points[1], points[2]);
	emitTriangle(points[1], points[3], points[2]);

	emitTriangle(points[4], points[6], points[5]);
	emitTriangle(points[5], points[6], points[7]);

	emitTriangle(points[1], points[0], points[4]);
	emitTriangle(points[1], points[4], points[5]);

	emitTriangle(points[3], points[6], points[2]);
	emitTriangle(points[3], points[7], points[6]);

	emitTriangle(points[0], points[2], points[4]);
	emitTriangle(points[4], points[2], points[6]);

	emitTriangle(points[1], points[5], points[3]);
	emitTriangle(points[5], points[7], points[3]);
}

vec4 transformVec(vec4 lineWorld)
{
	return projectionViewMatrix * lineWorld;
}

void emitTriangle(vec4 vertexClip0, vec4 vertexClip1, vec4 vertexClip2)
{
	gl_Position = gl_in[0].gl_Position + vertexClip0;
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vertexClip1;
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vertexClip2;
	EmitVertex();
	EndPrimitive();
}
