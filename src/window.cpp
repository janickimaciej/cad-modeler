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

	glfwSetFramebufferSizeCallback(m_windowPtr, callbackWrapper<&Window::resizeCallback>);
	glfwSetCursorPosCallback(m_windowPtr, callbackWrapper<&Window::cursorMovementCallback>);
	glfwSetScrollCallback(m_windowPtr, callbackWrapper<&Window::scrollCallback>);
	glfwSetMouseButtonCallback(m_windowPtr, callbackWrapper<&Window::buttonCallback>);
	glfwSetKeyCallback(m_windowPtr, callbackWrapper<&Window::keyCallback>);

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

void Window::resizeCallback(int width, int height)
{
	if (width == 0 || height == 0)
	{
		return;
	}

	m_size = {width, height};
	m_scene->updateWindowSize();
	glViewport(0, 0, width, height);
}

void Window::cursorMovementCallback(double x, double y)
{
	glm::vec2 currentPos{static_cast<float>(x), static_cast<float>(y)};
	glm::vec2 offset = currentPos - m_lastCursorPos;
	m_lastCursorPos = currentPos;

	if ((!isKeyPressed(GLFW_KEY_LEFT_SHIFT) &&
		isButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE))
		||
		(isKeyPressed(GLFW_KEY_LEFT_ALT) &&
		isButtonPressed(GLFW_MOUSE_BUTTON_LEFT)))
	{
		static constexpr float sensitivity = 0.002f;
		m_scene->addPitchCamera(-sensitivity * offset.y);
		m_scene->addYawCamera(sensitivity * offset.x);
	}

	if ((isKeyPressed(GLFW_KEY_LEFT_SHIFT) &&
		isButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE))
		||
		(isKeyPressed(GLFW_KEY_RIGHT_SHIFT) &&
		isButtonPressed(GLFW_MOUSE_BUTTON_LEFT)))
	{
		static constexpr float sensitivity = 0.001f;
		m_scene->moveXCamera(-sensitivity * offset.x);
		m_scene->moveYCamera(sensitivity * offset.y);
	}

	if (isKeyPressed(GLFW_KEY_RIGHT_ALT) &&
		isButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
	{
		static constexpr float sensitivity = 1.005f;
		m_scene->zoomCamera(std::pow(sensitivity, -offset.y));
	}

	if (!isKeyPressed(GLFW_KEY_LEFT_ALT) &&
		!isKeyPressed(GLFW_KEY_RIGHT_ALT) &&
		!isKeyPressed(GLFW_KEY_RIGHT_SHIFT) &&
		isButtonPressed(GLFW_MOUSE_BUTTON_LEFT) &&
		m_dragging)
	{
		m_scene->moveUniqueSelectedModel(offset);
	}
}

void Window::scrollCallback(double, double yOffset)
{
	if (isCursorInGUI())
	{
		return;
	}

	static constexpr float sensitivity = 1.1f;
	m_scene->zoomCamera(std::pow(sensitivity, static_cast<float>(yOffset)));
}

void Window::buttonCallback(int button, int action, int)
{
	glm::vec2 cursorPos = getCursorPos();

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS &&
		!isKeyPressed(GLFW_KEY_LEFT_ALT) &&
		!isKeyPressed(GLFW_KEY_RIGHT_ALT) &&
		!isKeyPressed(GLFW_KEY_RIGHT_SHIFT))
	{
		if (isCursorInGUI())
		{
			return;
		}

		if (isKeyPressed(GLFW_KEY_LEFT_CONTROL))
		{
			m_scene->toggleModel(cursorPos);
		}
		else
		{
			if (m_scene->selectUniqueModel(cursorPos))
			{
				m_dragging = true;
			}
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		m_dragging = false;
	}
}

void Window::keyCallback(int key, int, int action, int)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		m_gui->cancel();
		m_scene->deselectAllModels();
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		m_gui->apply();
	}

	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
	{
		m_gui->startRenaming();
	}

	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
	{
		m_gui->deleteSelectedModels();
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		m_scene->centerCamera();
	}

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		if (m_rotatingRequested)
		{
			m_gui->startRotatingX();
		}

		if (m_scalingRequested)
		{
			m_gui->startScalingX();
		}
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		if (m_rotatingRequested)
		{
			m_gui->startRotatingY();
		}

		if (m_scalingRequested)
		{
			m_gui->startScalingY();
		}
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		if (m_rotatingRequested)
		{
			m_gui->startRotatingZ();
		}

		if (m_scalingRequested)
		{
			m_gui->startScalingZ();
		}
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		m_rotatingRequested = true;
	}
	else if (action == GLFW_PRESS)
	{
		m_rotatingRequested = false;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS &&
		!isKeyPressed(GLFW_KEY_LEFT_CONTROL))
	{
		m_scalingRequested = true;
	}
	else if (action == GLFW_PRESS)
	{
		m_scalingRequested = false;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS &&
		isKeyPressed(GLFW_KEY_LEFT_CONTROL))
	{
		m_gui->startSerializing();
	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS &&
		isKeyPressed(GLFW_KEY_LEFT_CONTROL))
	{
		m_gui->startDeserializing();
	}
}

glm::vec2 Window::getCursorPos() const
{
	double x{};
	double y{};
	glfwGetCursorPos(m_windowPtr, &x, &y);
	return {static_cast<float>(x), static_cast<float>(y)};
}

bool Window::isButtonPressed(int button)
{
	return glfwGetMouseButton(m_windowPtr, button) == GLFW_PRESS;
}

bool Window::isKeyPressed(int key)
{
	return glfwGetKey(m_windowPtr, key) == GLFW_PRESS;
}

bool Window::isCursorInGUI()
{
	glm::vec2 cursorPos = getCursorPos();
	return cursorPos.x <= LeftPanel::width || cursorPos.x >= m_size.x - RightPanel::width;
}
