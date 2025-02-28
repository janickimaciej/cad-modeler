#include "window.hpp"

#include "guis/leftPanel.hpp"
#include "guis/rightPanel.hpp"

#include <cmath>
#include <string>

Window::Window(const glm::ivec2& initialSize) :
	m_size{initialSize}
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	static const std::string windowTitle = "cad-opengl";
	m_windowPtr = glfwCreateWindow(initialSize.x, initialSize.y, windowTitle.c_str(), nullptr,
		nullptr);
	glfwSetWindowUserPointer(m_windowPtr, this);
	glfwMakeContextCurrent(m_windowPtr);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(m_windowPtr, resizeCallback);
	glfwSetCursorPosCallback(m_windowPtr, cursorMovementCallback);
	glfwSetScrollCallback(m_windowPtr, scrollCallback);
	glfwSetMouseButtonCallback(m_windowPtr, buttonCallback);
	glfwSetKeyCallback(m_windowPtr, keyCallback);

	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
}

Window::~Window()
{
	glfwTerminate();
}

const glm::ivec2& Window::size() const
{
	return m_size;
}

void Window::setWindowData(Scene& scene, GUI& gui)
{
	m_scene = &scene;
	m_gui = &gui;
}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_windowPtr);
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

glm::vec2 Window::cursorPos() const
{
	double x{};
	double y{};
	glfwGetCursorPos(m_windowPtr, &x, &y);
	return {static_cast<float>(x), static_cast<float>(y)};
}

void Window::resizeCallback(GLFWwindow* windowPtr, int width, int height)
{
	if (width == 0 || height == 0)
	{
		return;
	}

	Window* window = getWindow(windowPtr);

	window->m_size = {width, height};
	window->m_scene->updateWindowSize();
	glViewport(0, 0, width, height);
}

void Window::cursorMovementCallback(GLFWwindow* windowPtr, double x, double y)
{
	Window* window = getWindow(windowPtr);

	glm::vec2 currentPos{static_cast<float>(x), static_cast<float>(y)};
	glm::vec2 offset{currentPos - window->m_lastCursorPos};
	window->m_lastCursorPos = currentPos;

	if ((isKeyPressed(windowPtr, GLFW_KEY_LEFT_ALT) &&
		isButtonPressed(windowPtr, GLFW_MOUSE_BUTTON_LEFT))
		||
		(!isKeyPressed(windowPtr, GLFW_KEY_LEFT_ALT) &&
		!isKeyPressed(windowPtr, GLFW_KEY_RIGHT_ALT) &&
		!isKeyPressed(windowPtr, GLFW_KEY_LEFT_SHIFT) &&
		!isKeyPressed(windowPtr, GLFW_KEY_RIGHT_SHIFT) &&
		isButtonPressed(windowPtr, GLFW_MOUSE_BUTTON_MIDDLE)))
	{
		static constexpr float sensitivity = 0.002f;
		window->m_scene->addPitchCamera(-sensitivity * offset.y);
		window->m_scene->addYawCamera(sensitivity * offset.x);
	}

	if ((isKeyPressed(windowPtr, GLFW_KEY_LEFT_SHIFT) ||
		isKeyPressed(windowPtr, GLFW_KEY_RIGHT_SHIFT)) &&
		isButtonPressed(windowPtr, GLFW_MOUSE_BUTTON_MIDDLE))
	{
		static constexpr float sensitivity = 0.001f;
		window->m_scene->moveXCamera(-sensitivity * offset.x);
		window->m_scene->moveYCamera(sensitivity * offset.y);
	}

	if (isKeyPressed(windowPtr, GLFW_KEY_RIGHT_ALT) &&
		isButtonPressed(windowPtr, GLFW_MOUSE_BUTTON_LEFT))
	{
		static constexpr float sensitivity = 1.005f;
		window->m_scene->zoomCamera(std::pow(sensitivity, -offset.y));
	}

	if (!isKeyPressed(windowPtr, GLFW_KEY_LEFT_ALT) &&
		!isKeyPressed(windowPtr, GLFW_KEY_RIGHT_ALT) &&
		!isKeyPressed(windowPtr, GLFW_KEY_LEFT_SHIFT) &&
		!isKeyPressed(windowPtr, GLFW_KEY_RIGHT_SHIFT) &&
		isButtonPressed(windowPtr, GLFW_MOUSE_BUTTON_LEFT) &&
		window->m_dragging)
	{
		window->m_scene->moveUniqueSelectedModel(currentPos);
	}
}

void Window::scrollCallback(GLFWwindow* windowPtr, double, double yOffset)
{
	Window* window = getWindow(windowPtr);

	glm::vec2 cursorPos = window->cursorPos();
	if (isCursorInGUI(*window))
	{
		return;
	}

	static constexpr float sensitivity = 1.1f;
	window->m_scene->zoomCamera(std::pow(sensitivity, static_cast<float>(yOffset)));
}

void Window::buttonCallback(GLFWwindow* windowPtr, int button, int action, int)
{
	Window* window = getWindow(windowPtr);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		glm::vec2 cursorPos = window->cursorPos();
		if (isCursorInGUI(*window))
		{
			return;
		}

		if (isKeyPressed(windowPtr, GLFW_KEY_LEFT_CONTROL))
		{
			window->m_scene->toggleModel(cursorPos);
		}
		else
		{
			if (window->m_scene->selectUniqueModel(cursorPos))
			{
				window->m_dragging = true;
			}
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		window->m_dragging = false;
	}
}

void Window::keyCallback(GLFWwindow* windowPtr, int key, int, int action, int)
{
	Window* window = getWindow(windowPtr);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		window->m_gui->cancel();
		window->m_scene->deselectAllModels();
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		window->m_gui->apply();
	}

	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
	{
		window->m_gui->startRenaming();
	}

	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
	{
		window->m_gui->deleteSelectedModels();
	}

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		if (window->m_rotatingRequested)
		{
			window->m_gui->startRotatingX();
		}

		if (window->m_scalingRequested)
		{
			window->m_gui->startScalingX();
		}
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		if (window->m_rotatingRequested)
		{
			window->m_gui->startRotatingY();
		}

		if (window->m_scalingRequested)
		{
			window->m_gui->startScalingY();
		}
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		if (window->m_rotatingRequested)
		{
			window->m_gui->startRotatingZ();
		}

		if (window->m_scalingRequested)
		{
			window->m_gui->startScalingZ();
		}
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		window->m_rotatingRequested = true;
	}
	else if (action == GLFW_PRESS)
	{
		window->m_rotatingRequested = false;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS &&
		!isKeyPressed(windowPtr, GLFW_KEY_LEFT_CONTROL))
	{
		window->m_scalingRequested = true;
	}
	else if (action == GLFW_PRESS)
	{
		window->m_scalingRequested = false;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS &&
		isKeyPressed(windowPtr, GLFW_KEY_LEFT_CONTROL))
	{
		window->m_gui->startSerializing();
	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS &&
		isKeyPressed(windowPtr, GLFW_KEY_LEFT_CONTROL))
	{
		window->m_gui->startDeserializing();
	}
}

Window* Window::getWindow(GLFWwindow* windowPtr)
{
	return static_cast<Window*>(glfwGetWindowUserPointer(windowPtr));
}

bool Window::isButtonPressed(GLFWwindow* windowPtr, int button)
{
	return glfwGetMouseButton(windowPtr, button) == GLFW_PRESS;
}

bool Window::isKeyPressed(GLFWwindow* windowPtr, int key)
{
	return glfwGetKey(windowPtr, key) == GLFW_PRESS;
}

bool Window::isCursorInGUI(const Window& window)
{
	glm::vec2 cursorPos = window.cursorPos();
	return cursorPos.x <= LeftPanel::width || cursorPos.x >= window.m_size.x - RightPanel::width;
}
