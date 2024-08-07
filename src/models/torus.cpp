#include "models/torus.hpp"

#include <glm/gtc/constants.hpp>

#include <cmath>

static constexpr float initialMajorRadius = 3.0f;
static constexpr float initialMinorRadius = 1.0f;
static constexpr int initialMajor = 32;
static constexpr int initialMinor = 16;

Torus::Torus(const ShaderProgram& shaderProgram, const glm::vec3& pos) :
	Model{pos, "Torus " + std::to_string(m_count)},
	m_id{m_count++},
	m_shaderProgram{shaderProgram},
	m_gui{*this},
	m_majorRadius{initialMajorRadius},
	m_minorRadius{initialMinorRadius},
	m_majorGrid{initialMajor},
	m_minorGrid{initialMinor}
{
	updateMesh();
}

void Torus::render() const
{
	updateShaders();
	m_mesh->render();
}

void Torus::updateGUI()
{
	m_gui.update();
}

float Torus::getMajorRadius() const
{
	return m_majorRadius;
}

void Torus::setMajorRadius(float majorRadius)
{
	m_majorRadius = majorRadius;
	updateMesh();
}

float Torus::getMinorRadius() const
{
	return m_minorRadius;
}

void Torus::setMinorRadius(float minorRadius)
{
	m_minorRadius = minorRadius;
	updateMesh();
}

int Torus::getMajorGrid() const
{
	return m_majorGrid;
}

void Torus::setMajorGrid(int majorGrid)
{
	m_majorGrid = majorGrid;
	updateMesh();
}

int Torus::getMinorGrid() const
{
	return m_minorGrid;
}

void Torus::setMinorGrid(int minorGrid)
{
	m_minorGrid = minorGrid;
	updateMesh();
}

int Torus::m_count = 0;

void Torus::updateShaders() const
{
	m_shaderProgram.use();
	m_shaderProgram.setUniform("modelMatrix", m_modelMatrix);
	m_shaderProgram.setUniform("isSelected", isSelected());
}

void Torus::updateMesh()
{
	m_mesh = std::make_unique<Mesh>(createVertices(), createIndices());
}

std::vector<glm::vec3> Torus::createVertices() const
{
	std::vector<glm::vec3> vertices{};
	
	static constexpr float pi = glm::pi<float>();
	float ds = 2 * pi / m_majorGrid;
	float dt = 2 * pi / m_minorGrid;
	for (int is = 0; is < m_majorGrid; ++is)
	{
		for (int it = 0; it < m_minorGrid; ++it)
		{
			float s = is * ds;
			float t = it * dt;

			vertices.push_back(getSurfacePos(s, t));
		}
	}

	return vertices;
}

std::vector<unsigned int> Torus::createIndices() const
{
	std::vector<unsigned int> indices{};

	for (int i = 0; i < m_majorGrid; ++i)
	{
		for (int j = 0; j < m_minorGrid; ++j)
		{
			int ind0 = i * m_minorGrid + j;
			int ind1 = i * m_minorGrid + (j + 1) % m_minorGrid;
			int ind2 = ((i + 1) % m_majorGrid) * m_minorGrid + j;

			indices.push_back(ind0);
			indices.push_back(ind1);

			indices.push_back(ind0);
			indices.push_back(ind2);
		}
	}

	return indices;
}

glm::vec3 Torus::getSurfacePos(float s, float t) const
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
