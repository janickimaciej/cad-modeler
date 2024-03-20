#include "models/torus.hpp"

#include <glm/gtc/constants.hpp>

#include <cmath>
#include <vector>

constexpr float initialMajorRadius = 3.0f;
constexpr float initialMinorRadius = 1.0f;
constexpr int initialMajor = 32;
constexpr int initialMinor = 16;

Torus::Torus(const ShaderProgram& wireframeShaderProgram, const ShaderProgram& solidShaderProgram,
	glm::vec3 position) :
	Model{position, "Torus " + std::to_string(m_count)},
	m_id{m_count++},
	m_wireframeShaderProgram{wireframeShaderProgram},
	m_solidShaderProgram{solidShaderProgram},
	m_gui{*this},
	m_majorRadius{initialMajorRadius},
	m_minorRadius{initialMinorRadius},
	m_major{initialMajor},
	m_minor{initialMinor}
{
	updateMesh();
}

void Torus::render(RenderMode renderMode) const
{
	updateShaders(renderMode);
	m_mesh->render(renderMode);
}

ModelGUI& Torus::getGUI()
{
	return m_gui;
}

float Torus::getMajorRadius() const
{
	return m_majorRadius;
}

float Torus::getMinorRadius() const
{
	return m_minorRadius;
}

int Torus::getMajor() const
{
	return m_major;
}

int Torus::getMinor() const
{
	return m_minor;
}

void Torus::setMajorRadius(float majorRadius)
{
	if (majorRadius != m_majorRadius)
	{
		m_majorRadius = majorRadius;
		updateMesh();
	}
}

void Torus::setMinorRadius(float minorRadius)
{
	if (minorRadius != m_minorRadius)
	{
		m_minorRadius = minorRadius;
		updateMesh();
	}
}

void Torus::setMajor(int major)
{
	if (major != m_major)
	{
		m_major = major;
		updateMesh();
	}
}

void Torus::setMinor(int minor)
{
	if (minor != m_minor)
	{
		m_minor = minor;
		updateMesh();
	}
}

int Torus::m_count = 0;

void Torus::updateShaders(RenderMode renderMode) const
{
	switch (renderMode)
	{
	case RenderMode::wireframe:
		m_wireframeShaderProgram.use();
		m_wireframeShaderProgram.setUniformMatrix4f("modelMatrix", m_modelMatrix);
		m_wireframeShaderProgram.setUniform1b("isActive", isActive());
		break;

	case RenderMode::solid:
		m_solidShaderProgram.use();
		m_solidShaderProgram.setUniformMatrix4f("modelMatrix", m_modelMatrix);
		m_solidShaderProgram.setUniform1b("isActive", isActive());
		break;
	}
}

void Torus::updateMesh()
{
	m_mesh.reset();
	m_mesh = std::make_unique<Mesh>(createVertices(), createIndicesWireframe(),
		createIndicesSolid());
}

std::vector<Vertex> Torus::createVertices() const
{
	std::vector<Vertex> vertices{};
	
	float ds = 2 * glm::pi<float>() / m_major;
	float dt = 2 * glm::pi<float>() / m_minor;
	for (int is = 0; is < m_major; ++is)
	{
		for (int it = 0; it < m_minor; ++it)
		{
			float s = is * ds;
			float t = it * dt;

			vertices.push_back(
				Vertex
				{
					getSurfacePosition(s, t),
					getSurfaceNormalVector(s, t)
				}
			);
		}
	}

	return vertices;
}

std::vector<unsigned int> Torus::createIndicesWireframe() const
{
	std::vector<unsigned int> indices{};

	for (int i = 0; i < m_major; ++i)
	{
		for (int j = 0; j < m_minor; ++j)
		{
			int ind0 = i * m_minor + j;
			int ind1 = i * m_minor + (j + 1) % m_minor;
			int ind2 = ((i + 1) % m_major) * m_minor + j;

			indices.push_back(ind0);
			indices.push_back(ind1);

			indices.push_back(ind0);
			indices.push_back(ind2);
		}
	}

	return indices;
}

std::vector<unsigned int> Torus::createIndicesSolid() const
{
	std::vector<unsigned int> indices{};
	
	for (int i = 0; i < m_major; ++i)
	{
		for (int j = 0; j < m_minor; ++j)
		{
			int ind0 = i * m_minor + j;
			int ind1 = i * m_minor + (j + 1) % m_minor;
			int ind2 = ((i + 1) % m_major) * m_minor + j;
			int ind3 = ((i + 1) % m_major) * m_minor + (j + 1) % m_minor;

			indices.push_back(ind1);
			indices.push_back(ind0);
			indices.push_back(ind2);

			indices.push_back(ind1);
			indices.push_back(ind2);
			indices.push_back(ind3);
		}
	}

	return indices;
}

glm::vec3 Torus::getSurfacePosition(float s, float t) const
{
	float common = m_minorRadius * std::cos(t) + m_majorRadius;

	return
		glm::vec3
		{
			std::sin(s) * common,
			m_minorRadius * std::sin(t),
			std::cos(s) * common
		};
}

glm::vec3 Torus::getSurfaceNormalVector(float s, float t) const
{
	float sins = std::sin(s);
	float coss = std::cos(s);
	float sint = std::sin(t);
	float cost = std::cos(t);

	return glm::normalize(
		glm::vec3
		{
			sins * cost,
			(sins * sins + coss * coss) * sint,
			coss * cost
		}
	);
}
