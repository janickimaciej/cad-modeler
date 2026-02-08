#pragma once

#include "cameras/camera.hpp"
#include "gui/cursorGUI.hpp"
#include "meshes/pointMesh.hpp"

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
	CursorGUI m_gui{*this};

	glm::vec3 m_pos{0, 0, 0};

	PointMesh m_mesh{};

	void updateShaders() const;
};
