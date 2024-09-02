#pragma once

#include "guis/guiMode.hpp"
#include "guis/leftPanel.hpp"
#include "guis/valueWindows/deserializingWindow.hpp"
#include "guis/valueWindows/renamingWindow.hpp"
#include "guis/valueWindows/rotatingWindow.hpp"
#include "guis/valueWindows/scalingWindow.hpp"
#include "guis/valueWindows/serializingWindow.hpp"
#include "models/model.hpp"
#include "scene.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

class GUI
{
public:
	GUI(GLFWwindow* window, Scene& scene, const glm::ivec2& windowSize);
	~GUI();

	void update();
	void render();
	void startRotatingX();
	void startRotatingY();
	void startRotatingZ();
	void startScalingX();
	void startScalingY();
	void startScalingZ();
	void startRenaming();
	void startSerializing();
	void startDeserializing();
	void cancel();
	void apply();
	void deleteSelectedModels();
	void setWindowSize(const glm::ivec2& windowSize);

private:
	LeftPanel m_leftPanel;
	RotatingWindow m_rotatingWindow;
	ScalingWindow m_scalingWindow;
	RenamingWindow m_renamingWindow;
	SerializingWindow m_serializingWindow;
	DeserializingWindow m_deserializingWindow;

	GUIMode m_mode = GUIMode::none;
	Scene& m_scene;
	
	void startRotating(GUIMode mode);
	void startScaling(GUIMode mode);
};
