#pragma once

#include "guis/centerPointGUI.hpp"
#include "models/model.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

class CenterPoint
{
public:
	CenterPoint();
	void render(const ShaderProgram& shaderProgram);
	void updateGUI();

	std::vector<Model*> getModels() const;
	int getModelCount() const;
	void addModel(Model* model);
	void deleteModel(const Model* model);
	void deleteAllModels();

	glm::vec3 getPos() const;
	void setPos(const glm::vec3& pos);

	void rotateX(float angleRad);
	void rotateY(float angleRad);
	void rotateZ(float angleRad);

	void scaleX(float scale);
	void scaleY(float scale);
	void scaleZ(float scale);

private:
	CenterPointGUI m_gui;
	std::vector<Model*> m_models{};
	glm::vec3 m_pos{0, 0, 0};
	unsigned int m_VAO{};

	void rotate(const glm::mat3& rotationMatrix);
	void updatePos();
	void updateShaders(const ShaderProgram& shaderProgram);
	static glm::vec3 matrixToEuler(const glm::mat3& rotationMatrix);
};
