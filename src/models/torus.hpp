#pragma once

#include "mesh.hpp"
#include "model_guis/model_gui.hpp"
#include "model_guis/torus_gui.hpp"
#include "models/model.hpp"
#include "render_mode.hpp"
#include "shader_program.hpp"
#include "vertex.hpp"

#include <glm/glm.hpp>

#include <memory>

class Torus : public Model
{
public:
	Torus(float majorRadius, float minorRadius, int major, int minor);
	virtual void render(RenderMode renderMode, const ShaderProgram& shaderProgram) const override;
	virtual ModelGUI& getGUI() override;

	float getMajorRadius() const;
	float getMinorRadius() const;
	int getMajor() const;
	int getMinor() const;

	void setMajorRadius(float majorRadius);
	void setMinorRadius(float minorRadius);
	void setMajor(int major);
	void setMinor(int minor);

private:
	std::unique_ptr<Mesh> m_mesh{};
	TorusGUI m_gui;

	float m_majorRadius{};
	float m_minorRadius{};

	int m_major{};
	int m_minor{};

	void updateMesh();
	std::vector<Vertex> createVertices() const;
	std::vector<unsigned int> createIndicesWireframe() const;
	std::vector<unsigned int> createIndicesSolid() const;

	glm::vec3 getPosition(float s, float t) const;
	glm::vec3 getNormalVector(float s, float t) const;
};
