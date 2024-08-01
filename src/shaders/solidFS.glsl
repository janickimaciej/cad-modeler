#version 400 core

in vec3 pos;
in vec3 unnormalizedNormalVector;

uniform vec3 cameraPos;
uniform float ambient;
uniform float diffuse;
uniform float specular;
uniform float shininess;
uniform bool isActive;

out vec4 outColor;

void main()
{
	float brightness = ambient;

	vec3 normalVector = normalize(unnormalizedNormalVector);
	vec3 viewVector = normalize(cameraPos - pos);
	vec3 lightVector = viewVector;

	float lightNormalCos = dot(lightVector, normalVector);
	brightness += lightNormalCos > 0 ? diffuse * lightNormalCos : 0;

	vec3 reflectionVector = 2 * dot(lightVector, normalVector) * normalVector - lightVector;
	float reflectionViewCos = dot(reflectionVector, viewVector);
	brightness += reflectionViewCos > 0 ? specular * pow(reflectionViewCos, shininess) : 0;

	outColor =
		isActive ? vec4(brightness, brightness, 0, 1) : vec4(brightness, brightness, brightness, 1);
}
