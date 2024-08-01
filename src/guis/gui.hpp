#pragma once

#include "guis/gui_state.hpp"
#include "models/model.hpp"
#include "scene.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

class GUI
{
public:
	GUI(GLFWwindow* window, Scene& scene, int windowWidth, int windowHeight);
	~GUI();

	void update();
	void render();
	void startRenaming();
	void startRotatingX();
	void startRotatingY();
	void startRotatingZ();
	void startScalingX();
	void startScalingY();
	void startScalingZ();
	void cancel();
	void apply();
	void deleteActiveModels();
	void setWindowSize(int width, int height);

private:
	Scene& m_scene;
	std::string m_renderMode{};
	std::string m_cameraType{};

	int m_windowWidth{};
	int m_windowHeight{};

	GUIState m_state = GUIState::none;

	Model* m_uniqueActiveModel{};
	static constexpr int maxNameLength = 32;
	std::array<char, maxNameLength> m_name{};
	bool m_focusFirstTime{};

	float m_rotationDeg{};
	float m_scale{};

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
