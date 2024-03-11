#include "models/torus.hpp"

#include "vertex.hpp"

#include <glm/gtc/constants.hpp>

#include <cmath>
#include <vector>

Torus::Torus(float majorRadius, float minorRadius, int major, int minor) :
	m_gui{*this},
	m_majorRadius{majorRadius},
	m_minorRadius{minorRadius},
	m_major{major},
	m_minor{minor}
{
	updateMesh();
}

void Torus::render(const ShaderProgram& shaderProgram) const
{
	updateShaders(shaderProgram);
	m_mesh->render();
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

void Torus::updateMesh()
{
	m_mesh.reset();
	std::vector<Vertex> vertices{};
	
	float dt = 2 * glm::pi<float>() / m_minor;
	float ds = 2 * glm::pi<float>() / m_major;
	for (int it = 0; it < m_minor; ++it)
	{
		for (int is = 0; is < m_major; ++is)
		{
			float t = it * dt;
			float s = is * ds;
			vertices.push_back(
				Vertex
				{
					getPosition(t, s),
					glm::vec2{},
					getNormalVector(t, s)
				}
			);
		}
	}

	m_mesh = std::make_unique<Mesh>(vertices);
}

glm::vec3 Torus::getPosition(float t, float s) const
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

glm::vec3 Torus::getNormalVector(float t, float s) const
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
