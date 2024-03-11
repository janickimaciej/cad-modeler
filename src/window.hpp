#pragma once

#include "scene.hpp"
#include "window_user_data.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

class Window
{
public:
	Window(int width, int height);
	void setScene(Scene& scene);
	bool shouldClose();
	void pollEvents();
	void swapBuffers();
	GLFWwindow* getPtr();
	static void resizeCallback(GLFWwindow* window, int width, int height);
	static void cursorMovementCallback(GLFWwindow* window, double x, double y);
	static void scrollCallback(GLFWwindow* window, double, double yOffset);
	~Window();

private:
	GLFWwindow* m_windowPtr{};
	WindowUserData m_userData{};
};
