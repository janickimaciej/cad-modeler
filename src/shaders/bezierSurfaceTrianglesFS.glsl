#version 420 core

in vec3 pos;
in vec3 normalVector;

uniform vec3 cameraPos;

out vec4 outColor;

void main()
{
	const float ambient = 0.2f;
	const float diffuse = 0.4f;
	const float specular = 0.4f;
	const float shininess = 20.0f;

	float brightness = ambient;

	vec3 normalizedNormalVector = normalize(normalVector);
	vec3 viewVector = normalize(cameraPos - pos);
	vec3 lightVector = viewVector;

	float lightNormalCos = dot(lightVector, normalizedNormalVector);
	brightness += lightNormalCos > 0 ? diffuse * lightNormalCos : 0;

	vec3 reflectionVector =
		2 * dot(lightVector, normalizedNormalVector) * normalizedNormalVector - lightVector;
	float reflectionViewCos = dot(reflectionVector, viewVector);
	brightness += reflectionViewCos > 0 ? specular * pow(reflectionViewCos, shininess) : 0;

	outColor = vec4(pos.y, brightness, brightness, 1);
}
