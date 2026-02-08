#include "models/torus.hpp"

#include "shaderPrograms.hpp"

#include <glm/gtc/constants.hpp>

#include <cmath>

static constexpr float initialMajorRadius = 1.0f;
static constexpr float initialMinorRadius = 0.3f;
static constexpr int initialMajor = 32;
static constexpr int initialMinor = 16;

Torus::Torus(const Intersectable::ChangeCallback& changeCallback, const glm::vec3& pos) :
	Intersectable{pos, "Torus " + std::to_string(m_count++), changeCallback},
	m_majorRadius{initialMajorRadius},
	m_minorRadius{initialMinorRadius},
	m_majorGrid{initialMajor},
	m_minorGrid{initialMinor}
{
	createMesh();
}

void Torus::render() const
{
	useTrim(*ShaderPrograms::torus);
	updateShaders();
	m_mesh->render();
}

void Torus::updateGUI()
{
	m_gui.update();
}

void Torus::setPos(const glm::vec3& pos)
{
	Model::setPos(pos);
	notifyChange();
}

void Torus::setPitchRad(float pitchRad)
{
	Model::setPitchRad(pitchRad);
	notifyChange();
}

void Torus::setYawRad(float yawRad)
{
	Model::setYawRad(yawRad);
	notifyChange();
}

void Torus::setRollRad(float rollRad)
{
	Model::setRollRad(rollRad);
	notifyChange();
}

void Torus::setScale(const glm::vec3& scale)
{
	Model::setScale(scale);
	notifyChange();
}

float Torus::getMajorRadius() const
{
	return m_majorRadius;
}

void Torus::setMajorRadius(float majorRadius)
{
	m_majorRadius = majorRadius;
	updateMesh();
	notifyChange();
}

float Torus::getMinorRadius() const
{
	return m_minorRadius;
}

void Torus::setMinorRadius(float minorRadius)
{
	m_minorRadius = minorRadius;
	updateMesh();
	notifyChange();
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

glm::vec3 Torus::surfaceLocal(float u, float v) const
{
	static constexpr float pi = glm::pi<float>();
	float uScaled = 2 * pi * u;
	float vScaled = 2 * pi * v;
	float common = m_minorRadius * std::cos(vScaled) + m_majorRadius;
	return glm::vec3
	{
		std::sin(uScaled) * common,
		m_minorRadius * std::sin(vScaled),
		std::cos(uScaled) * common
	};
}

glm::vec3 Torus::surface(float u, float v) const
{
	return glm::vec3{getModelMatrix() * glm::vec4{surfaceLocal(u, v), 1}};
}

glm::vec3 Torus::surfaceDU(float u, float v) const
{
	static constexpr float pi = glm::pi<float>();
	float uScaled = 2 * pi * u;
	float vScaled = 2 * pi * v;
	float common = m_minorRadius * std::cos(vScaled) + m_majorRadius;
	glm::vec3 surfaceDULocal
	{
		2 * pi * std::cos(uScaled) * common,
		0,
		-2 * pi * std::sin(uScaled) * common
	};

	return glm::vec3{getModelMatrix() * glm::vec4{surfaceDULocal, 0}};
}

glm::vec3 Torus::surfaceDV(float u, float v) const
{
	static constexpr float pi = glm::pi<float>();
	float uScaled = 2 * pi * u;
	float vScaled = 2 * pi * v;
	float common = -2 * pi * m_minorRadius * std::sin(vScaled);
	glm::vec3 surfaceDVLocal
	{
		std::sin(uScaled) * common,
		2 * pi * m_minorRadius * std::cos(vScaled),
		std::cos(uScaled) * common
	};

	return glm::vec3{getModelMatrix() * glm::vec4{surfaceDVLocal, 0}};
}

bool Torus::uWrapped() const
{
	return true;
}

bool Torus::vWrapped() const
{
	return true;
}

int Torus::m_count = 0;

void Torus::createMesh()
{
	m_mesh = std::make_unique<TorusMesh>(createVertices(), createIndices());
}

void Torus::updateShaders() const
{
	ShaderPrograms::torus->use();
	ShaderPrograms::torus->setUniform("modelMatrix", getModelMatrix());
	ShaderPrograms::torus->setUniform("isDark", false);
	ShaderPrograms::torus->setUniform("isSelected", isSelected());
	ShaderPrograms::torus->setUniform("textureMin", glm::vec2{0, 0});
	ShaderPrograms::torus->setUniform("textureMax", glm::vec2{1, 1});
}

void Torus::updateMesh()
{
	m_mesh->update(createVertices(), createIndices());
}

std::vector<TorusMesh::Vertex> Torus::createVertices() const
{
	std::vector<TorusMesh::Vertex> vertices{};

	float du = 1.0f / m_majorGrid;
	float dv = 1.0f / m_minorGrid;
	for (int iu = 0; iu <= m_majorGrid; ++iu)
	{
		for (int iv = 0; iv <= m_minorGrid; ++iv)
		{
			float u = iu * du;
			float v = iv * dv;

			vertices.push_back({surfaceLocal(u, v), {u, v}});
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
			int ind0 = i * (m_minorGrid + 1) + j;
			int ind1 = i * (m_minorGrid + 1) + j + 1;
			int ind2 = (i + 1) * (m_minorGrid + 1) + j;

			indices.push_back(ind0);
			indices.push_back(ind1);

			indices.push_back(ind0);
			indices.push_back(ind2);
		}
	}

	return indices;
}
