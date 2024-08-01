#pragma once

#include "guis/modelGUIs/modelGUI.hpp"
#include "guis/modelGUIs/torusGUI.hpp"
#include "mesh.hpp"
#include "models/model.hpp"
#include "renderMode.hpp"
#include "shaderProgram.hpp"
#include "vertex.hpp"

#include <glm/glm.hpp>

#include <memory>

class Torus : public Model
{
public:
	Torus(const ShaderProgram& wireframeShaderProgram, const ShaderProgram& solidShaderProgram,
		glm::vec3 position);
	virtual void render(RenderMode renderMode) const override;
	virtual void updateGUI() override;

	float getMajorRadius() const;
	void setMajorRadius(float majorRadius);
	float getMinorRadius() const;
	void setMinorRadius(float minorRadius);
	int getMajor() const;
	void setMajor(int major);
	int getMinor() const;
	void setMinor(int minor);

private:
	static int m_count;
	int m_id{};

	const ShaderProgram& m_wireframeShaderProgram;
	const ShaderProgram& m_solidShaderProgram;

	std::unique_ptr<Mesh> m_mesh{};
	TorusGUI m_gui;

	float m_majorRadius{};
	float m_minorRadius{};

	int m_major{};
	int m_minor{};

	virtual void updateShaders(RenderMode renderMode) const override;

	void updateMesh();
	std::vector<Vertex> createVertices() const;
	std::vector<unsigned int> createIndicesWireframe() const;
	std::vector<unsigned int> createIndicesSolid() const;

	glm::vec3 getSurfacePosition(float s, float t) const;
	glm::vec3 getSurfaceNormalVector(float s, float t) const;
};
