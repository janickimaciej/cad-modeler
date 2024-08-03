#include "window.hpp"

#include <glm/glm.hpp>

#include <string>

Window::Window(int width, int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	static const std::string windowTitle = "cad-opengl";
	m_windowPtr = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_windowPtr, &m_windowData);
	glfwMakeContextCurrent(m_windowPtr);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(m_windowPtr, resizeCallback);
	glfwSetCursorPosCallback(m_windowPtr, cursorMovementCallback);
	glfwSetScrollCallback(m_windowPtr, scrollCallback);
	glfwSetMouseButtonCallback(m_windowPtr, buttonCallback);
	glfwSetKeyCallback(m_windowPtr, keyCallback);

	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
}

Window::~Window()
{
	glfwTerminate();
}

void Window::setWindowData(Scene& scene, GUI& gui)
{
	m_windowData.scene = &scene;
	m_windowData.gui = &gui;
}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_windowPtr);
}

void Window::clear() const
{
	static constexpr glm::vec3 backgroundColor{0.1f, 0.1f, 0.1f};
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::swapBuffers() const
{
	glfwSwapBuffers(m_windowPtr);
}

void Window::pollEvents() const
{
	glfwPollEvents();
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

	WindowData* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
	windowData->scene->setWindowSize(width, height);
	windowData->gui->setWindowSize(width, height);
	glViewport(0, 0, width, height);
}

void Window::cursorMovementCallback(GLFWwindow* window, double x, double y)
{
	WindowData* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

	glm::vec2 currentPos{static_cast<float>(x), static_cast<float>(y)};
	glm::vec2 offset{currentPos - windowData->lastCursorPos};
	windowData->lastCursorPos = currentPos;

	if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) ||
		(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS))
	{
		static constexpr float sensitivity = 0.002f;
		windowData->scene->addPitchCamera(-sensitivity * offset.y);
		windowData->scene->addYawCamera(sensitivity * offset.x);
	}

	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		static constexpr float sensitivity = 0.001f;
		windowData->scene->moveXCamera(-sensitivity * offset.x);
		windowData->scene->moveYCamera(sensitivity * offset.y);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		static constexpr float sensitivity = 1.005f;
		windowData->scene->zoomCamera(std::pow(sensitivity, -offset.y));
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
		windowData->dragging)
	{
		windowData->scene->moveUniqueActiveModel(currentPos);
	}
}

void Window::buttonCallback(GLFWwindow* window, int button, int action, int)
{
	WindowData* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double x{};
		double y{};
		glfwGetCursorPos(window, &x, &y);
		glm::vec2 cursorPos{static_cast<float>(x), static_cast<float>(y)};
		
		bool toggle = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

		if (windowData->scene->select(cursorPos, toggle) && !toggle)
		{
			windowData->dragging = true;
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		windowData->dragging = false;
	}
}

void Window::scrollCallback(GLFWwindow* window, double, double yOffset)
{
	WindowData* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

	static constexpr float sensitivity = 1.1f;
	windowData->scene->zoomCamera(std::pow(sensitivity, static_cast<float>(yOffset)));
}

void Window::keyCallback(GLFWwindow* window, int key, int, int action, int)
{
	WindowData* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		windowData->gui->cancel();
		windowData->scene->clearActiveModels();
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		windowData->gui->apply();
	}

	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
	{
		windowData->gui->startRenaming();
	}

	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
	{
		windowData->gui->deleteActiveModels();
	}

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		if (windowData->rotatingRequested)
		{
			windowData->gui->startRotatingX();
		}

		if (windowData->scalingRequested)
		{
			windowData->gui->startScalingX();
		}
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		if (windowData->rotatingRequested)
		{
			windowData->gui->startRotatingY();
		}

		if (windowData->scalingRequested)
		{
			windowData->gui->startScalingY();
		}
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		if (windowData->rotatingRequested)
		{
			windowData->gui->startRotatingZ();
		}

		if (windowData->scalingRequested)
		{
			windowData->gui->startScalingZ();
		}
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		windowData->rotatingRequested = true;
	}
	else if (action == GLFW_PRESS)
	{
		windowData->rotatingRequested = false;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		windowData->scalingRequested = true;
	}
	else if (action == GLFW_PRESS)
	{
		windowData->scalingRequested = false;
	}
}
