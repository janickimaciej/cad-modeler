#include "shader_program.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

constexpr std::size_t errorLogSize = 512;

ShaderProgram::ShaderProgram(const std::string& vertexShaderPath,
	const std::string& fragmentShaderPath)
{
	unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderPath);
	unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderPath);
	m_id = createShaderProgram(vertexShader, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void ShaderProgram::use() const
{
	glUseProgram(m_id);
}

void ShaderProgram::setUniform1b(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), static_cast<int>(value));
}

void ShaderProgram::setUniform1i(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::setUniform1f(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::setUniform3f(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniformMatrix4f(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE,
		glm::value_ptr(value));
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_id);
}

unsigned int ShaderProgram::createShader(GLenum shaderType, const std::string& shaderPath) const
{
	std::string shaderCode = readShaderFile(shaderPath);

	unsigned int shader = glCreateShader(shaderType);
	const char* shaderCodeCStr = shaderCode.c_str();
	glShaderSource(shader, 1, &shaderCodeCStr, NULL);
	glCompileShader(shader);
	int success{};
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		printCompilationError(shaderType, shader);
	}
	return shader;
}

unsigned int ShaderProgram::createShaderProgram(unsigned int vertexShader,
	unsigned int fragmentShader) const
{
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	int success{};
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		printLinkingError(shaderProgram);
	}
	return shaderProgram;
}

std::string ShaderProgram::readShaderFile(const std::string& shaderPath) const
{
	std::ifstream file{};
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::string shaderCode{};

	try
	{
		std::stringstream stream{};
		file.open(shaderPath);
		stream << file.rdbuf();
		file.close();
		shaderCode = stream.str();
	}
	catch (std::ifstream::failure)
	{
		std::cerr << "Error reading file:\n" << shaderPath << '\n';
	}

	return shaderCode;
}

void ShaderProgram::printCompilationError(GLenum shaderType, unsigned int shaderId) const
{
	std::array<char, errorLogSize> errorLog{};
	glGetShaderInfoLog(shaderId, errorLogSize, nullptr, errorLog.data());
	std::string shaderTypeName = (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment");
	std::cerr << "Error compiling " + std::string(shaderTypeName) + " shader:\n" <<
		errorLog.data() << '\n';
}

void ShaderProgram::printLinkingError(unsigned int programId) const
{
	std::array<char, errorLogSize> errorLog{};
	glGetProgramInfoLog(programId, errorLogSize, nullptr, errorLog.data());
	std::cerr << "Error linking shader program:\n" << errorLog.data() << '\n';
}
