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

	glm::vec2 cursorPos() const;

	static void resizeCallback(GLFWwindow* windowPtr, int width, int height);
	static void cursorMovementCallback(GLFWwindow* windowPtr, double x, double y);
	static void scrollCallback(GLFWwindow* windowPtr, double, double yOffset);
	static void buttonCallback(GLFWwindow* windowPtr, int button, int action, int);
	static void keyCallback(GLFWwindow* windowPtr, int key, int, int action, int);

	static Window* getWindow(GLFWwindow* windowPtr);
	static bool isButtonPressed(GLFWwindow* windowPtr, int button);
	static bool isKeyPressed(GLFWwindow* windowPtr, int key);
	static bool isCursorInGUI(const Window& window);
};
