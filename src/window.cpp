#include "window.hpp"

#include "guis/gui.hpp"
#include "scene.hpp"
#include "window_user_data.hpp"

#include <string>

Window::Window(int width, int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	const std::string windowTitle = "cad-opengl";
	m_windowPtr = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_windowPtr, &m_userData);
	glfwMakeContextCurrent(m_windowPtr);
	glfwSetFramebufferSizeCallback(m_windowPtr, resizeCallback);
	glfwSetCursorPosCallback(m_windowPtr, cursorMovementCallback);
	glfwSetScrollCallback(m_windowPtr, scrollCallback);
	glfwSetMouseButtonCallback(m_windowPtr, buttonCallback);
	glfwSetKeyCallback(m_windowPtr, keyCallback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::setUserData(Scene& scene, GUI& gui)
{
	m_userData.scene = &scene;
	m_userData.gui = &gui;
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(m_windowPtr);
}

void Window::clear() const
{
	constexpr glm::vec3 backgroundColor{0.1f, 0.1f, 0.1f};
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

	WindowUserData* userData = static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));
	userData->scene->setAspectRatio(static_cast<float>(width) / height);
	userData->scene->setWindowSize(width, height);
	userData->gui->setWindowSize(width, height);
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
		windowUserData->scene->addPitchCamera(-sensitivity * yOffset);
		windowUserData->scene->addYawCamera(sensitivity * xOffset);
	}

	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		float sensitivity = 0.001f;
		windowUserData->scene->moveXCamera(-sensitivity * xOffset);
		windowUserData->scene->moveYCamera(sensitivity * yOffset);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS &&
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		float sensitivity = 1.005f;
		windowUserData->scene->zoomCamera(std::pow(sensitivity, -yOffset));
	}
}

void Window::buttonCallback(GLFWwindow* window, int button, int action, int)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xPos{};
		double yPos{};
		glfwGetCursorPos(window, &xPos, &yPos);

		
		WindowUserData* windowUserData =
			static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));
		windowUserData->scene->activate(static_cast<float>(xPos), static_cast<float>(yPos),
			glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
	}
}

void Window::scrollCallback(GLFWwindow* window, double, double yOffset)
{
	WindowUserData* windowUserData =
		static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));

	float sensitivity = 1.1f;
	windowUserData->scene->zoomCamera(std::pow(sensitivity, static_cast<float>(yOffset)));
}

void Window::keyCallback(GLFWwindow* window, int key, int, int action, int)
{
	WindowUserData* windowUserData =
		static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		windowUserData->gui->cancel();
		windowUserData->scene->clearActiveModels();
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		windowUserData->gui->apply();
	}

	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
	{
		windowUserData->gui->startRenaming();
	}

	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
	{
		windowUserData->gui->deleteActiveModels();
	}

	static bool rotatingRequested = false;
	static bool scalingRequested = false;

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		if (rotatingRequested)
		{
			windowUserData->gui->startRotatingX();
		}

		if (scalingRequested)
		{
			windowUserData->gui->startScalingX();
		}
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		if (rotatingRequested)
		{
			windowUserData->gui->startRotatingY();
		}

		if (scalingRequested)
		{
			windowUserData->gui->startScalingY();
		}
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		if (rotatingRequested)
		{
			windowUserData->gui->startRotatingZ();
		}

		if (scalingRequested)
		{
			windowUserData->gui->startScalingZ();
		}
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		rotatingRequested = true;
	}
	else if (action == GLFW_PRESS)
	{
		rotatingRequested = false;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		scalingRequested = true;
	}
	else if (action == GLFW_PRESS)
	{
		scalingRequested = false;
	}
}

Window::~Window()
{
	glfwTerminate();
}
