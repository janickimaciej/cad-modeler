#pragma once

#include "guis/gui.hpp"
#include "scene.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

class Window
{
public:
	Window(const glm::ivec2& initialSize);
	~Window();

	const glm::ivec2& size() const;
	void setWindowData(Scene& scene, GUI& gui);
	bool shouldClose() const;
	void swapBuffers() const;
	void pollEvents() const;
	GLFWwindow* getPtr();

private:
	GLFWwindow* m_windowPtr{};
	glm::ivec2 m_size{};
	Scene* m_scene{};
	GUI* m_gui{};

	glm::vec2 m_lastCursorPos{};
	bool m_dragging = false;
	bool m_rotatingRequested = false;
	bool m_scalingRequested = false;

	void resizeCallback(int width, int height);
	void cursorMovementCallback(double x, double y);
	void scrollCallback(double yOffset);
	void buttonCallback(int button, int action);
	void keyCallback(int key, int action);

	glm::vec2 getCursorPos() const;
	bool isButtonPressed(int button);
	bool isKeyPressed(int key);
	bool isCursorInGUI();

	static void resizeCallbackWrapper(GLFWwindow* windowPtr, int width, int height);
	static void cursorMovementCallbackWrapper(GLFWwindow* windowPtr, double x, double y);
	static void scrollCallbackWrapper(GLFWwindow* windowPtr, double, double yOffset);
	static void buttonCallbackWrapper(GLFWwindow* windowPtr, int button, int action, int);
	static void keyCallbackWrapper(GLFWwindow* windowPtr, int key, int, int action, int);

	static Window* getWindow(GLFWwindow* windowPtr);
};
