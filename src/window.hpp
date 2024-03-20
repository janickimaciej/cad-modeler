#pragma once

#include "scene.hpp"
#include "window_user_data.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

class Window
{
public:
	Window(int width, int height);
	void setUserData(Scene& scene, GUI& gui);
	bool shouldClose();
	void clear() const;
	void swapBuffers() const;
	void pollEvents() const;
	GLFWwindow* getPtr();
	static void resizeCallback(GLFWwindow* window, int width, int height);
	static void cursorMovementCallback(GLFWwindow* window, double x, double y);
	static void scrollCallback(GLFWwindow* window, double, double yOffset);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	~Window();

private:
	GLFWwindow* m_windowPtr{};
	WindowUserData m_userData{};
};
