#version 330 core

in vec3 position;
in vec3 unnormalizedNormalVector;

uniform vec3 cameraPosition;
uniform float ambient;
uniform float diffuse;
uniform float specular;
uniform float shininess;

out vec4 outColor;

void main()
{
	float brightness = ambient;

	vec3 normalVector = normalize(unnormalizedNormalVector);
	vec3 viewVector = normalize(cameraPosition - position);
	vec3 lightVector = viewVector;

	float lightNormalCos = dot(lightVector, normalVector);
	brightness += lightNormalCos > 0 ? diffuse * lightNormalCos : 0;

	vec3 reflectionVector = 2 * dot(lightVector, normalVector) * normalVector - lightVector;
	float reflectionViewCos = dot(reflectionVector, viewVector);
	brightness += reflectionViewCos > 0 ? specular * pow(reflectionViewCos, shininess) : 0;

	outColor = vec4(brightness, brightness, brightness, 1);
}
