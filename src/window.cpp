#include "window.hpp"

#include "scene.hpp"
#include "window_user_data.hpp"

#include <string>
#include <iostream>
Window::Window(int width, int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	const std::string windowTitle = "cad-opengl";
	m_windowPtr = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_windowPtr, &m_userData);
	glfwMakeContextCurrent(m_windowPtr);
	glfwSetFramebufferSizeCallback(m_windowPtr, resizeCallback);
	glfwSetCursorPosCallback(m_windowPtr, cursorMovementCallback);
	glfwSetScrollCallback(m_windowPtr, scrollCallback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void Window::setScene(Scene& scene)
{
	m_userData.scene = &scene;
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(m_windowPtr);
}

void Window::pollEvents()
{
	glfwPollEvents();
}

void Window::swapBuffers()
{
	glfwSwapBuffers(m_windowPtr);
}

GLFWwindow* Window::getPtr()
{
	return m_windowPtr;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
	if (width == 0 || height == 0)
	{
		return;
	}

	WindowUserData* userData = static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));
	userData->scene->setAspectRatio(static_cast<float>(width) / height);
	glViewport(0, 0, width, height);
}

void Window::cursorMovementCallback(GLFWwindow* window, double x, double y)
{
	static float previousX{};
	static float previousY{};

	float xOffset = static_cast<float>(x) - previousX;
	float yOffset = static_cast<float>(y) - previousY;
	previousX = static_cast<float>(x);
	previousY = static_cast<float>(y);
	
	WindowUserData* windowUserData =
		static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));

	if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) ||
		(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS))
	{
		float sensitivity = 0.002f;
		windowUserData->scene->addPitch(-sensitivity * yOffset);
		windowUserData->scene->addYaw(sensitivity * xOffset);
	}

	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		float sensitivity = 0.001f;
		windowUserData->scene->moveX(-sensitivity * xOffset);
		windowUserData->scene->moveY(sensitivity * yOffset);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		float sensitivity = 1.005f;
		windowUserData->scene->zoom(std::pow(sensitivity, -yOffset));
	}
}

void Window::scrollCallback(GLFWwindow* window, double, double yOffset)
{
	WindowUserData* windowUserData =
		static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));

	float sensitivity = 1.1f;
	windowUserData->scene->zoom(std::pow(sensitivity, static_cast<float>(yOffset)));
}

Window::~Window()
{
	glfwTerminate();
}
