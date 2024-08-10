#pragma once

#include "guis/modelGUIs/torusGUI.hpp"
#include "meshes/mesh.hpp"
#include "models/model.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Torus : public Model
{
public:
	Torus(const ShaderProgram& shaderProgram, const glm::vec3& pos);
	virtual void render() const override;
	virtual void updateGUI() override;

	float getMajorRadius() const;
	void setMajorRadius(float majorRadius);
	float getMinorRadius() const;
	void setMinorRadius(float minorRadius);
	int getMajorGrid() const;
	void setMajorGrid(int majorGrid);
	int getMinorGrid() const;
	void setMinorGrid(int minorGrid);

private:
	static int m_count;

	const ShaderProgram& m_shaderProgram;

	std::unique_ptr<Mesh> m_mesh{};
	TorusGUI m_gui{*this};

	float m_majorRadius{};
	float m_minorRadius{};

	int m_majorGrid{};
	int m_minorGrid{};

	void createMesh();

	virtual void updateShaders() const override;

	void updateMesh();
	std::vector<glm::vec3> createVertices() const;
	std::vector<unsigned int> createIndices() const;

	glm::vec3 getSurfacePos(float s, float t) const;
};
