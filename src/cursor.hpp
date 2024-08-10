#pragma once

#include "guis/cursorGUI.hpp"
#include "meshes/pointMesh.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

class Cursor
{
public:
	Cursor(const ShaderProgram& shaderProgram);

	void render() const;
	void updateGUI(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize);

	glm::vec3 getPos() const;
	void setPos(const glm::vec3& pos);
	glm::vec2 getScreenPos(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize) const;
	void setScreenPos(const glm::vec2& screenPos, const glm::mat4& cameraMatrix,
		const glm::ivec2& windowSize);

private:
	const ShaderProgram& m_shaderProgram;

	CursorGUI m_gui{*this};

	glm::vec3 m_pos{0, 0, 0};

	PointMesh m_mesh{};

	void updateShaders() const;
};
