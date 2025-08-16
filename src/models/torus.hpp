#pragma once

#include "guis/modelGUIs/torusGUI.hpp"
#include "meshes/indicesMesh.hpp"
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
		const glm::vec3& pos);
	virtual ~Torus() = default;
	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3& pos) override;

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
	std::unique_ptr<IndicesMesh> m_mesh{};
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
};
