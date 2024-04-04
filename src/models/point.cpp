#include "models/point.hpp"

#include <glad/glad.h>

#include <array>
#include <string>

Point::Point(const Scene& scene, const ShaderProgram& wireframePointShaderProgram,
	const ShaderProgram& solidPointShaderProgram, glm::vec3 position) :
	Model{scene, position, "Point " + std::to_string(m_count)},
	m_id{m_count++},
	m_wireframePointShaderProgram{wireframePointShaderProgram},
	m_solidPointShaderProgram{solidPointShaderProgram},
	m_gui{*this}
{
	glGenVertexArrays(1, &m_VAO);
}

Point::~Point()
{
	notify(m_destroyNotifications);
}

void Point::render(RenderMode renderMode) const
{
	updateShaders(renderMode);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

ModelGUI& Point::getGUI()
{
	return m_gui;
}

void Point::setPosition(const glm::vec3& position)
{
	if (position != m_position)
	{
		Model::setPosition(position);
		notify(m_moveNotifications);
	}
}

void Point::setScreenPosition(const glm::vec2& screenPosition)
{
	Model::setScreenPosition(screenPosition);
	notify(m_moveNotifications);
}

std::shared_ptr<Point::Callback> Point::registerForMoveNotification(const Callback& callback)
{
	std::shared_ptr<Callback> notification = std::make_shared<Callback>(callback);
	m_moveNotifications.push_back(notification);
	return notification;
}

std::shared_ptr<Point::Callback> Point::registerForDestroyNotification(const Callback& callback)
{
	std::shared_ptr<Callback> notification = std::make_shared<Callback>(callback);
	m_destroyNotifications.push_back(notification);
	return notification;
}

int Point::m_count = 0;

void Point::updateShaders(RenderMode renderMode) const
{
	switch (renderMode)
	{
	case RenderMode::wireframe:
		/*m_wireframePointShaderProgram.use();
		m_wireframePointShaderProgram.setUniform3f("posWorld", m_position);
		m_wireframePointShaderProgram.setUniform1b("isActive", isActive());
		break;*/

	case RenderMode::solid:
		m_solidPointShaderProgram.use();
		m_solidPointShaderProgram.setUniform3f("posWorld", m_position);
		m_solidPointShaderProgram.setUniform1b("isActive", isActive());
		break;
	}
}

void Point::notify(std::vector<std::weak_ptr<Callback>>& notifications)
{
	std::erase_if(notifications,
		[] (const std::weak_ptr<std::function<void(Point*)>>& notification)
		{
			return notification.expired();
		}
	);

	for (const std::weak_ptr<std::function<void(Point*)>>& notification : notifications)
	{
		std::shared_ptr<std::function<void(Point*)>> notificationShared = notification.lock();
		if (notificationShared)
		{
			(*notificationShared)(this);
		}
	}
}
