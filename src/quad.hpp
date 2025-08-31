#pragma once

#include "shaderProgram.hpp"
#include "shaderPrograms.hpp"

class Quad
{
public:
	Quad();
	void render() const;

private:
	const ShaderProgram& m_shaderProgram = *ShaderPrograms::quad;

	unsigned int m_VBO{};
	unsigned int m_VAO{};
};
