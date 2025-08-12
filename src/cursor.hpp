#pragma once

#include "cameras/camera.hpp"
#include "guis/cursorGUI.hpp"
#include "meshes/pointMesh.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

class Cursor
{
public:
	Cursor(const ShaderProgram& shaderProgram);

	void render() const;
	void updateGUI(const Camera& camera);

	glm::vec3 getPos() const;
	void setPos(const glm::vec3& pos);

private:
	const ShaderProgram& m_shaderProgram;

	CursorGUI m_gui{*this};

	glm::vec3 m_pos{0, 0, 0};

	PointMesh m_mesh{};

	void updateShaders() const;
};
