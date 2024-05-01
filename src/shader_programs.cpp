#include "shader_programs.hpp"

std::string ShaderPrograms::path(const std::string& shaderName)
{
	return "src/shaders/" + shaderName + ".glsl";
}
