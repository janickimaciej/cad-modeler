#pragma once

#include "guis/gui.hpp"
#include "scene.hpp"
#include "window_user_data.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

class Window
{
public:
	Window(int width, int height);
	~Window();

	void setUserData(Scene& scene, GUI& gui);
	bool shouldClose() const;
	void clear() const;
	void swapBuffers() const;
	void pollEvents() const;
	GLFWwindow* getPtr();

private:
	GLFWwindow* m_windowPtr{};
	WindowUserData m_userData{};

	static void resizeCallback(GLFWwindow* window, int width, int height);
	static void cursorMovementCallback(GLFWwindow* window, double x, double y);
	static void buttonCallback(GLFWwindow* window, int button, int action, int);
	static void scrollCallback(GLFWwindow* window, double, double yOffset);
	static void keyCallback(GLFWwindow* window, int key, int, int action, int);
};
