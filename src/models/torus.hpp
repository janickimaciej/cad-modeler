#pragma once

#include "guis/modelGUIs/torusGUI.hpp"
#include "meshes/torusMesh.hpp"
#include "models/intersectable.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Torus : public Intersectable
{
	friend class TorusSerializer;

public:
	Torus(const Intersectable::ChangeCallback& changeCallback, const ShaderProgram& shaderProgram,
		const ShaderProgram& flatShaderProgram, const glm::vec3& pos);
	virtual ~Torus() = default;
	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3& pos) override;
	virtual void setPitchRad(float pitchRad) override;
	virtual void setYawRad(float yawRad) override;
	virtual void setRollRad(float rollRad) override;
	virtual void setScale(const glm::vec3& scale) override;

	float getMajorRadius() const;
	void setMajorRadius(float majorRadius);
	float getMinorRadius() const;
	void setMinorRadius(float minorRadius);
	int getMajorGrid() const;
	void setMajorGrid(int majorGrid);
	int getMinorGrid() const;
	void setMinorGrid(int minorGrid);

	glm::vec3 surfaceLocal(float u, float v) const;
	virtual glm::vec3 surface(float u, float v) const override;
	virtual glm::vec3 surfaceDU(float u, float v) const override;
	virtual glm::vec3 surfaceDV(float u, float v) const override;

	virtual bool uWrapped() const override;
	virtual bool vWrapped() const override;

private:
	static int m_count;

	const ShaderProgram& m_shaderProgram;
	std::unique_ptr<TorusMesh> m_mesh{};
	TorusGUI m_gui{*this};

	float m_majorRadius{};
	float m_minorRadius{};

	int m_majorGrid{};
	int m_minorGrid{};

	void createMesh();

	virtual void updateShaders() const override;

	void updateMesh();
	std::vector<TorusMesh::Vertex> createVertices() const;
	std::vector<unsigned int> createIndices() const;
};
