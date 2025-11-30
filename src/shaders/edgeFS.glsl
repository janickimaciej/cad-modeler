#version 420 core

in vec2 texturePos;

uniform float level;
uniform ivec2 resolution;
uniform sampler2D textureSampler;

out vec4 outColor;

const vec2 offsets[4] = vec2[4](vec2(-1, 0), vec2(1, 0), vec2(0, -1), vec2(0, 1));

bool isZero(vec2 texPos)
{
	const float eps = 1e-6;
	return texture(textureSampler, texPos).r < level + eps;
}

bool isEdge(vec2 texPos)
{
	vec2 resolutionInv = 1.0 / resolution;
	bool hasNonZeroNeighbor = false;
	for (int i = 0; i < 4; ++i)
	{
		vec2 neighborPos = texPos + resolutionInv * offsets[i];
		hasNonZeroNeighbor = hasNonZeroNeighbor || !isZero(neighborPos);
	}
	return isZero(texPos) && hasNonZeroNeighbor;
}

void main()
{
	vec2 resolutionInv = 1.0 / resolution;
	bool edge = isEdge(texturePos);
	bool hasZeroNonEdgeNeighbor = false;
	for (int i = 0; i < 4; ++i)
	{
		vec2 neighborPos = texturePos + resolutionInv * offsets[i];
		hasZeroNonEdgeNeighbor = hasZeroNonEdgeNeighbor ||
			(isZero(neighborPos) && !isEdge(neighborPos));
	}
	
	outColor = vec4(edge && hasZeroNonEdgeNeighbor, 0, 0, 1);
}
