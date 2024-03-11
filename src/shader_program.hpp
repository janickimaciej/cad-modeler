#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <string_view>

class ShaderProgram
{
public:
	ShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&&) = delete;
	void use() const;
	void setUniform1b(const std::string& name, bool value) const;
	void setUniform1i(const std::string& name, int value) const;
	void setUniform1f(const std::string& name, float value) const;
	void setUniform3f(const std::string& name, const glm::vec3& value) const;
	void setUniformMatrix4f(const std::string& name, const glm::mat4& value) const;
	~ShaderProgram();

private:
	unsigned int m_id{};

	unsigned int createShader(GLenum shaderType, const std::string& shaderPath) const;
	unsigned int createShaderProgram(unsigned int vertexShader,
		unsigned int fragmentShader) const;
	std::string readShaderFile(const std::string& shaderPath) const;
	void printCompilationError(GLenum shaderType, unsigned int shaderId) const;
	void printLinkingError(unsigned int programId) const;
};
