#pragma once

#include "models/model.hpp"
#include "scene.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

class GUI
{
public:
	GUI(GLFWwindow* window, Scene& scene, int windowWidth, int windowHeight);
	void update();
	void render();
	void startRenaming();
	void stopRenaming();
	void deleteActiveModels();
	void setWindowSize(int width, int height);
	~GUI();

private:
	Scene& m_scene;
	std::string m_renderMode{};
	std::string m_cameraType{};

	int m_windowWidth{};
	int m_windowHeight{};

	bool m_renaming = false;
	Model* m_uniqueActiveModel{};
	static constexpr int maxNameLength = 32;
	std::array<char, maxNameLength> m_name{};
	bool m_renamingFocusFirstTime{};

	void getValues();
	void setValues();

	void renderMode();
	void cameraType();
	void camera();
	void cursor();
	void activeModelsCenter();
	void buttons();
	void modelList();

	void separator();
};
