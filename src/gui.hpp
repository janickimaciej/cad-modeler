#pragma once

#include "scene.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

class GUI
{
public:
	GUI(GLFWwindow* window, Scene& scene);
	void update();
	void render();
	~GUI();

private:
	Scene& m_scene;
	std::string m_renderMode{};
	std::string m_cameraType{};

	void getValues();
	void setValues();
	void separator();
};
