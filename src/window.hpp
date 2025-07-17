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
	void scrollCallback(double, double yOffset);
	void buttonCallback(int button, int action, int);
	void keyCallback(int key, int, int action, int);

	glm::vec2 getCursorPos() const;
	bool isButtonPressed(int button);
	bool isKeyPressed(int key);
	bool isCursorInGUI();

	static Window* getWindow(GLFWwindow* windowPtr);

	template <auto callback, typename... Args>
	static void callbackWrapper(GLFWwindow* windowPtr, Args... args)
	{
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowPtr));
		(window->*callback)(args...);
	}
};
