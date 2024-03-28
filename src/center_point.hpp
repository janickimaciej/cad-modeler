#pragma once

#include "guis/center_point_gui.hpp"
#include "models/model.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

#include <vector>

class CenterPoint
{
public:
	CenterPoint();
	void render(const ShaderProgram& shaderProgram) const;
	CenterPointGUI& getGUI();
	void setModels(const std::vector<Model*>& models);

	glm::vec3 getPosition();
	void setPosition(const glm::vec3& position);

	void rotateX(float angleRad);
	void rotateY(float angleRad);
	void rotateZ(float angleRad);

	void scaleX(float scale);
	void scaleY(float scale);
	void scaleZ(float scale);

private:
	CenterPointGUI m_gui;

	std::vector<Model*> m_models{};

	glm::vec3 m_position{0, 0, 0};

	unsigned int m_VAO{};

	void updatePosition();
	void updateShaders(const ShaderProgram& shaderProgram) const;
	static glm::vec3 matrixToEuler(const glm::mat3& rotationMatrix);
};
