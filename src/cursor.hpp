#pragma once

#include "cameras/camera.hpp"
#include "guis/cursorGUI.hpp"
#include "meshes/pointMesh.hpp"
#include "shaderProgram.hpp"
#include "shaderPrograms.hpp"

#include <glm/glm.hpp>

class Cursor
{
public:
	Cursor();

	void render() const;
	void updateGUI(const Camera& camera);

	glm::vec3 getPos() const;
	void setPos(const glm::vec3& pos);

private:
	const ShaderProgram& m_shaderProgram = *ShaderPrograms::cursor;

	CursorGUI m_gui{*this};

	glm::vec3 m_pos{0, 0, 0};

	PointMesh m_mesh{};

	void updateShaders() const;
};
