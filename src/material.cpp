#include "material.hpp"

#include <glm/glm.hpp>

Material::Material(const glm::ivec3& color, float ambientCoef, float diffuseCoef,
	float specularCoef, float shininess) :
	color{color},
	ambientCoef{ambientCoef},
	diffuseCoef{diffuseCoef},
	specularCoef{specularCoef},
	shininess{shininess}
{ }
