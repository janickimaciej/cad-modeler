#include "shaderPrograms.hpp"

std::string ShaderPrograms::path(const std::string& shaderName)
{
	return "src/shaders/" + shaderName + ".glsl";
}
